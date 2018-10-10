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

#ifndef DOWNLOADPREPLANNEDMAPAREAS_H
#define DOWNLOADPREPLANNEDMAPAREAS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class PreplannedMapAreaListModel;
class PreplannedMapArea;
class OfflineMapTask;
class DownloadPreplannedOfflineMapJob;
}
}

class QAbstractItemModel;

#include <QQuickItem>
#include <QTemporaryDir>

class DownloadPreplannedMapAreas : public QQuickItem
{
  Q_PROPERTY(QAbstractItemModel* areaList READ areaList NOTIFY areaListChanged)
  Q_PROPERTY(int jobProgress READ jobProgress NOTIFY jobProgressChanged)
  Q_OBJECT

public:
  explicit DownloadPreplannedMapAreas(QQuickItem* parent = nullptr);
  ~DownloadPreplannedMapAreas();

  void componentComplete() override;
  static void init();

  QAbstractItemModel* areaList() const;

  int jobProgress() const;

public slots:
  void downloadPreplannedMapArea(int rowIndex,
                                 const QString& downloadPath);

  void downloadPreplannedMapArea(Esri::ArcGISRuntime::PreplannedMapArea* area,
                                 const QString& downloadPath);

  void clearMapRegistration(Esri::ArcGISRuntime::Map* map);

signals:
  void areaListChanged();
  void jobProgressChanged(int progress);
  void jobFailed(const QString& message, const QString& details);

private:
  QTemporaryDir* m_mapDirectory = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::PreplannedMapAreaListModel* m_areaList = nullptr;
  Esri::ArcGISRuntime::OfflineMapTask* m_mapTask = nullptr;
  Esri::ArcGISRuntime::DownloadPreplannedOfflineMapJob* m_downloadJob = nullptr;
};

#endif // DOWNLOADPREPLANNEDMAPAREAS_H
