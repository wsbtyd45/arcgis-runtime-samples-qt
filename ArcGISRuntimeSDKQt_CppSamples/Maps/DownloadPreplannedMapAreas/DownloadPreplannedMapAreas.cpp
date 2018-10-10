// [WriteFile Name=DownloadPreplannedMapAreas, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "DownloadPreplannedMapAreas.h"

#include "Map.h"
#include "MapQuickView.h"
#include "PortalItem.h"
#include "OfflineMapTask.h"
#include "PreplannedMapAreaListModel.h"
#include "DownloadPreplannedOfflineMapJob.h"
#include "GeodatabaseFeatureTable.h"
#include "Geodatabase.h"
#include "FeatureLayer.h"
#include "GeodatabaseSyncTask.h"

#include <QFileInfo>

using namespace Esri::ArcGISRuntime;

DownloadPreplannedMapAreas::DownloadPreplannedMapAreas(QQuickItem* parent):
  QQuickItem(parent),
  m_mapDirectory(new QTemporaryDir())
{
}

DownloadPreplannedMapAreas::~DownloadPreplannedMapAreas()
{
  clearMapRegistration(m_mapView->map());
  delete m_mapDirectory;
}

void DownloadPreplannedMapAreas::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DownloadPreplannedMapAreas>(
        "Esri.Samples", 1, 0, "DownloadPreplannedMapAreasSample");
}

void DownloadPreplannedMapAreas::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  PortalItem* portalItem = new PortalItem("acc027394bc84c2fb04d1ed317aac674",
                                          this);

  m_mapTask = new OfflineMapTask(portalItem, this);

  connect(m_mapTask, &OfflineMapTask::preplannedMapAreasCompleted, this,
  [this]
  {
    qDebug() << "Area list created";
    m_areaList = m_mapTask->preplannedMapAreaList();
    emit areaListChanged();
  });

  qDebug() << "Prepping area list";
  m_mapTask->preplannedMapAreas();
}

void DownloadPreplannedMapAreas::downloadPreplannedMapArea(
    int rowIndex,
    const QString& downloadPath)
{
  qDebug() << "INDEX: " << rowIndex;
  PreplannedMapArea* area = m_areaList->at(rowIndex);
  downloadPreplannedMapArea(area, downloadPath);
}

void DownloadPreplannedMapAreas::downloadPreplannedMapArea(
    PreplannedMapArea* area,
    const QString& file)
{
  clearMapRegistration(m_mapView->map());

  const QString filePath = QDir::cleanPath(m_mapDirectory->filePath(file));

  m_downloadJob = m_mapTask->downloadPreplannedOfflineMap(area, filePath);
  connect(m_downloadJob, &DownloadPreplannedOfflineMapJob::jobStatusChanged,
          this,
    [this]()
    {
      if (m_downloadJob->jobStatus() == JobStatus::Succeeded)
      {
        m_mapView->setMap(m_downloadJob->result()->offlineMap());
        for (const auto& m : m_downloadJob->messages())
        {
          qDebug() << m.message();
        }
      }
      else if (m_downloadJob->jobStatus() == JobStatus::Failed)
      {
        const Error e = m_downloadJob->error();
        emit jobFailed(e.message(), e.additionalMessage());
      }
    }
  );

  connect(m_downloadJob, &DownloadPreplannedOfflineMapJob::progressChanged,
          this, [this] { emit jobProgressChanged(m_downloadJob->progress()); });

  m_downloadJob->start();
}

void DownloadPreplannedMapAreas::clearMapRegistration(Map* map)
{
  if (map == nullptr)
  {
    return;
  }

  QHash<QString, Geodatabase*> geodatabasesByPath;

  {
    // First discover all the tables with non-spatial data, and add their
    // geodatabases to our master list.
    FeatureTableListModel* tables = map->tables();
    const int rows = tables->rowCount();
    for (int i = 0; i < rows; ++i)
    {
      GeodatabaseFeatureTable* geodatabaseTable =
          static_cast<GeodatabaseFeatureTable*>(tables->at(i));
      if (geodatabaseTable != nullptr)
      {
        Geodatabase* geodatabase = geodatabaseTable->geodatabase();
        geodatabasesByPath.insert(geodatabase->path(), geodatabase);
      }
    }
  }

  {
    // Next go through all our layers, find the tables with spatial data, and
    // add their geodatabases to our master list.
    LayerListModel* operationalLayers = map->operationalLayers();
    const int rows = operationalLayers->rowCount();
    for (int i = 0; i < rows; ++i)
    {
      Layer* layer = operationalLayers->at(i);
      if (layer->layerType() != LayerType::FeatureLayer)
        continue;

      FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
      if (featureLayer != nullptr)
      {
        GeodatabaseFeatureTable* table =
            qobject_cast<GeodatabaseFeatureTable*>(featureLayer->featureTable());
        if (table != nullptr)
        {
          Geodatabase* geodatabase = table->geodatabase();
          geodatabasesByPath.insert(geodatabase->path(), geodatabase);
        }
      }
    }
  }

  for (Geodatabase* geodatabase : geodatabasesByPath)
  {
    GeodatabaseSyncTask* syncTask =
        new GeodatabaseSyncTask(geodatabase->serviceUrl(), this);

    connect(syncTask, &GeodatabaseSyncTask::unregisterGeodatabaseCompleted,
            this,
    [syncTask]
    {
      qDebug() << "Unregister completed";
      syncTask->deleteLater();
    });

    const QUuid syncId = geodatabase->syncId();
    connect(syncTask, &GeodatabaseSyncTask::doneLoading, this,
            [syncTask, syncId]
    {
      qDebug() << "Start unregister";
      syncTask->unregisterGeodatabase(syncId);

    });

    connect(syncTask, &GeodatabaseSyncTask::errorOccurred, this,
            [](Error e)
    {
      qDebug() << e.message();
      qDebug() << e.additionalMessage();
    });

    syncTask->load();

    geodatabase->close();
  }

  // Delete temp directory and re-create temp directory.
  delete m_mapDirectory;
  m_mapDirectory = new QTemporaryDir();
}

QAbstractItemModel* DownloadPreplannedMapAreas::areaList() const
{
  return m_areaList;
}

int DownloadPreplannedMapAreas::jobProgress() const
{
  if (m_downloadJob)
  {
    return m_downloadJob->progress();
  }
  return 0;
}
