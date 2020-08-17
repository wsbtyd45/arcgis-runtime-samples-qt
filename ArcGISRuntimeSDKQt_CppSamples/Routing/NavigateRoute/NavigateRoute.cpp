// [WriteFile Name=NavigateRoute, Category=Routing]
// [Legal]
// Copyright 2020 Esri.

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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "NavigateRoute.h"

#include "DirectionManeuverListModel.h"
#include "GeometryEngine.h"
#include "GraphicsOverlay.h"
#include "Location.h"
#include "LocationDisplay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "NavigationTypes.h"
#include "Point.h"
#include "PolylineBuilder.h"
#include "Route.h"
#include "RouteResult.h"
#include "RouteTask.h"
#include "RouteTracker.h"
#include "RouteTrackerLocationDataSource.h"
#include "SimpleMarkerSymbol.h"
#include "SimulatedLocationDataSource.h"
#include "SimulationParameters.h"
#include "Stop.h"
#include "TrackingDistance.h"
#include "TrackingProgress.h"
#include "TrackingStatus.h"
#include "VoiceGuidance.h"

#include <memory>
#include <QList>
#include <QTextToSpeech>
#include <QTime>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

namespace  {
const QUrl routeTaskUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route");
const Point conventionCenterPoint(-117.160386727, 32.706608, SpatialReference::wgs84());
const Point memorialPoint(-117.173034, 32.712327, SpatialReference::wgs84());
const Point aerospaceMuseumPoint(-117.147230, 32.730467, SpatialReference::wgs84());
const QString polylineJson("{\"paths\":[[[-13042226.3795109,3856415.2300094147],[-13042226.009634402,3856411.9011209253],[-13042197.159267493,3856405.6132204453],[-13042156.102976123,3856384.530260012],[-13042119.485202739,3856353.4606341105],[-13042093.223971322,3856334.9668091694],[-13042062.894098418,3856324.6102672019],[-13042039.961755492,3856295.0201472957],[-13042011.111388583,3856258.7722504106],[-13041974.123738701,3856253.9638559259],[-13041957.109419754,3856217.7159590409],[-13041933.807200328,3856208.4690465704],[-13041919.751893371,3856225.853242015],[-13041943.054112798,3856245.456696453],[-13041970.79485021,3856271.7179278699],[-13042001.124723114,3856298.7189122844],[-13042035.523237506,3856312.7742192396],[-13042064.373604413,3856352.351004614],[-13042112.457549261,3856386.0097660073],[-13042143.897051662,3856403.0240849536],[-13042189.391861018,3856445.5598823186],[-13042251.531112839,3856461.8344482649],[-13042289.998268716,3856505.4798751269],[-13042356.206162006,3856533.5904890378],[-13042389.864923399,3856574.6467804075],[-13042411.317760332,3856595.359864342],[-13042583.310332309,3856698.1855310132],[-13042770.097964225,3856817.6556401323],[-13042865.156224424,3856864.2600789848],[-13042933.213500226,3856898.288716869],[-13042985.366086561,3856935.2763667516],[-13043050.464350356,3857001.854136541],[-13043121.110761631,3857036.9924039296],[-13043205.072726866,3857072.8704243158],[-13043298.651481085,3857063.6235118434],[-13043370.407521857,3857055.4862288693],[-13043395.189247278,3857076.5691893026],[-13043465.465782056,3857076.9390658014],[-13043488.768001482,3857053.2669698764],[-13043535.002563836,3857067.3222768316],[-13043567.181819234,3857081.0077072885],[-13043600.100827638,3857095.0630142419],[-13043587.155150179,3857129.4615286328],[-13043616.745270086,3857167.1889315136],[-13043633.759589031,3857183.0936209629],[-13043640.417366019,3857434.2397636562],[-13043653.732919982,3857638.7814675043],[-13043421.450478718,3857629.164678535],[-13043287.555186158,3857632.8634435227],[-13043206.182356417,3857635.8224555133],[-13043111.493972717,3857629.164678534],[-13043003.120158561,3857611.0407300917],[-13042424.415480388,3857659.9068389949],[-13042234.530311404,3857617.2194754253],[-13042099.84431945,3857613.5395302898],[-13041876.103655247,3857615.0115083433],[-13041603.051726203,3857640.0351352636],[-13041590.539912758,3857891.7433825196],[-13041607.467660381,3858047.0370672313],[-13041572.140187083,3858069.1167380437],[-13041524.300900323,3858039.6771769607],[-13041485.293481888,3858052.1889904211],[-13041502.957218546,3858185.4030043157],[-13041469.1017233,3858316.4090511343],[-13041394.030842556,3858435.639273515],[-13041338.095676498,3858468.0227907058],[-13041261.552817684,3858611.5406509847],[-13041271.120675035,3858654.2280145548],[-13041197.521772355,3858851.4730738038],[-13041141.586606298,3858854.4170299121],[-13041089.331385393,3858981.0071425657],[-13041024.564351011,3859059.7579684621],[-13040828.055280799,3859159.8524761414],[-13040750.776432958,3859263.6269289586],[-13040663.929727774,3859358.5695134471],[-13040659.513793612,3859466.0239113993],[-13040687.48137664,3859470.439845562],[-13040742.680553671,3859501.3513846989],[-13040775.800059889,3859510.9192420505],[-13040772.120114753,3859584.5181447575]]],\"spatialReference\":{\"wkid\":102100,\"latestWkid\":3857}}");
}

NavigateRoute::NavigateRoute(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::navigationVector(this), this))
{
  m_routeOverlay = new GraphicsOverlay(this);
  m_routeTask = new RouteTask(routeTaskUrl, this);
  m_speaker = new QTextToSpeech(this);
}

NavigateRoute::~NavigateRoute() = default;

void NavigateRoute::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<NavigateRoute>("Esri.Samples", 1, 0, "NavigateRouteSample");
}

MapQuickView* NavigateRoute::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void NavigateRoute::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // add a graphic showing the simulated GPS signal
  Polyline pline = Polyline::fromJson(polylineJson);
  Graphic* lineGraphic = new Graphic(pline, new SimpleLineSymbol(SimpleLineSymbolStyle::DashDotDot, Qt::darkRed, 5, this), this);
  m_routeOverlay->graphics()->append(lineGraphic);

  m_mapView->graphicsOverlays()->append(m_routeOverlay);
  connectRouteTaskSignals();

  m_routeTask->load();

  // add graphics for the predefined stops
  SimpleMarkerSymbol* stopSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 20, this);
  m_routeOverlay->graphics()->append(new Graphic(conventionCenterPoint, stopSymbol, this));
  m_routeOverlay->graphics()->append(new Graphic(memorialPoint, stopSymbol, this));
  m_routeOverlay->graphics()->append(new Graphic(aerospaceMuseumPoint, stopSymbol, this));

  emit mapViewChanged();
}

void NavigateRoute::connectRouteTaskSignals()
{
  connect(m_routeTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, RouteResult routeResult)
  {
    if (routeResult.isEmpty())
      return;

    if (routeResult.routes().empty())
      return;

    m_routeResult = routeResult;
    m_route = m_routeResult.routes()[0];

    // adjust viewpoint to enclose the route with a 100 DPI padding
    m_mapView->setViewpointGeometry(m_route.routeGeometry(), 100);

    // create a graphic to show the route
    m_routeAheadGraphic = new Graphic(m_route.routeGeometry(), new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::blue, 5, this), this);

    // create a graphic to represent the route that's been traveled (initially empty).
    m_routeTraveledGraphic = new Graphic(Geometry(), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::cyan, 3, this), this);
    m_routeOverlay->graphics()->append(m_routeAheadGraphic);
    m_routeOverlay->graphics()->append(m_routeTraveledGraphic);

    m_navigationEnabled = true;
    emit navigationEnabledChanged();
  });

  connect(m_routeTask, &RouteTask::createDefaultParametersCompleted, this, [this](QUuid, RouteParameters defaultParameters)
  {
    // set values for parameters
    defaultParameters.setReturnStops(true);
    defaultParameters.setReturnDirections(true);
    defaultParameters.setReturnRoutes(true);
    defaultParameters.setOutputSpatialReference(SpatialReference::wgs84());

    Stop stop1(conventionCenterPoint);
    Stop stop2(memorialPoint);
    Stop stop3(aerospaceMuseumPoint);

    QList<Stop> stopsList = {stop1, stop2, stop3};
    defaultParameters.setStops(stopsList);

    m_routeTask->solveRoute(defaultParameters);
  });

  connect(m_routeTask, &RouteTask::doneLoading, this, [this](const Error& error)
  {
    if (error.isEmpty())
    {
      m_routeTask->createDefaultParameters();
    }
    else
    {
      qDebug() << error.message() << error.additionalMessage();
    }
  });
}

bool NavigateRoute::navigationEnabled() const
{
  return m_navigationEnabled;
}

bool NavigateRoute::recenterEnabled() const
{
  return m_recenterEnabled;
}

void NavigateRoute::startNavigation()
{
  // disable the navigation button
  m_navigationEnabled = false;
  emit navigationEnabledChanged();

  // get the directions for the route
  m_directions = m_route.directionManeuvers(this);

  // create a route tracker
  m_routeTracker = new RouteTracker(m_routeResult, 0, this);
  connectRouteTrackerSignals();

  // enable "recenter" button when location display is moved from nagivation mode
  connect(m_mapView->locationDisplay(), &LocationDisplay::autoPanModeChanged, this, [this](LocationDisplayAutoPanMode autoPanMode)
  {
    m_recenterEnabled = autoPanMode != LocationDisplayAutoPanMode::Navigation;
    emit recenterEnabledChanged();
  });

  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](const Location& location)
  {
//    m_routeTracker->trackLocation(location); // route tracker needs to be started if using SimulatedLocationDataSource for location display
  });

  // turn on map view's navigation mode
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);

  // add a data source for the location display
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 40.0, 0.0, 0.0, this); // set speed
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
//  m_simulatedLocationDataSource->setLocationsWithPolyline(m_route.routeGeometry(), simulationParameters);
  m_simulatedLocationDataSource->setLocationsWithPolyline(Polyline::fromJson(polylineJson), simulationParameters);
  m_routeTrackerLocationDataSource = new RouteTrackerLocationDataSource(m_routeTracker, m_simulatedLocationDataSource, this);
  m_mapView->locationDisplay()->setDataSource(m_routeTrackerLocationDataSource);
  m_routeTrackerLocationDataSource->start();
}

void NavigateRoute::connectRouteTrackerSignals()
{
  connect(m_routeTracker, &RouteTracker::newVoiceGuidance, this, [this](VoiceGuidance* rawVoiceGuidance)
  {
    auto voiceGuidance = std::unique_ptr<VoiceGuidance>(rawVoiceGuidance);
    m_speaker->say(voiceGuidance->text());
  });

  connect(m_routeTracker, &RouteTracker::trackingStatusChanged, this, [this](TrackingStatus* rawTrackingStatus)
  {
    auto trackingStatus = std::unique_ptr<TrackingStatus>(rawTrackingStatus);
    QString textString("Route status: \n");
    if (trackingStatus->destinationStatus() == DestinationStatus::NotReached || trackingStatus->destinationStatus() == DestinationStatus::Approaching)
    {
      textString += "Distance remaining: " + trackingStatus->routeProgress()->remainingDistance()->displayText() + " " +
          trackingStatus->routeProgress()->remainingDistance()->displayTextUnits().pluralDisplayName() + "\n";
      QTime time = QTime::fromMSecsSinceStartOfDay(trackingStatus->routeProgress()->remainingTime() * 60 * 1000); // convert time to milliseconds
      textString += "Time remaining: " + time.toString("hh:mm:ss") + "\n";

      // display next direction
      if (DirectionManeuverListModel* directionList = dynamic_cast<DirectionManeuverListModel*>(m_directions))
      {
        if (trackingStatus->currentManeuverIndex() + 1 < directionList->directionManeuvers().length())
        {
          textString += "Next direction: "  + directionList->directionManeuvers()[trackingStatus->currentManeuverIndex() + 1].directionText() + "\n";
        }
      }
      m_routeTraveledGraphic->setGeometry(trackingStatus->routeProgress()->traversedGeometry());
      m_routeAheadGraphic->setGeometry(trackingStatus->routeProgress()->remainingGeometry());
    }
    else if (trackingStatus->destinationStatus() == DestinationStatus::Reached)
    {
      textString += "Destination reached.\n";

      // set the route geometries to reflect the completed route
      m_routeAheadGraphic->setGeometry(Geometry());
      m_routeTraveledGraphic->setGeometry(trackingStatus->routeResult().routes()[0].routeGeometry());

      // navigate to next stop, if available
      if (trackingStatus->remainingDestinationCount() > 1)
      {
        m_routeTracker->switchToNextDestination();
      }
      else
      {
        m_routeTrackerLocationDataSource->stop();
      }
    }
    m_textString = textString;
    emit textStringChanged();
  });

  connect(m_routeTracker, &RouteTracker::trackLocationCompleted, this, [this](QUuid)
  {
    m_routeTracker->generateVoiceGuidance();
  });
}

QString NavigateRoute::textString() const
{
  return m_textString;
}

void NavigateRoute::recenterMap()
{
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
}
