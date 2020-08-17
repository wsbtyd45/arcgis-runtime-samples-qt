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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9
import QtQuick.Layouts 1.11
import QtPositioning 5.2
import Esri.samples 1.0

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url routeTaskUrl: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route"
    property var m_route: null
    property var m_routeResult: null
    property var directionListModel: null
    property string textString: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapNavigationVector {}
        }

        GraphicsOverlay {
            id: routeOverlay
            Graphic {
                id: routeAheadGraphic
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDash
                    color: "blue"
                    width: 5
                }
            }
            Graphic {
                id: routeTraveledGraphic
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "cyan"
                    width: 3
                }
            }
            Graphic {
                Point {
                    id: conventionCenterPoint
                    x: -117.160386727
                    y: 32.706608
                    SpatialReference {wkid: 4326}
                }
                symbol: stopSymbol
            }
            Graphic {
                Point {
                    id: memorialPoint
                    x: -117.173034
                    y: 32.712327
                    SpatialReference {wkid: 4326}
                }
                symbol: stopSymbol
            }
            Graphic {
                Point {
                    id: aerospaceMuseumPoint
                    x: -117.147230
                    y: 32.730467
                    SpatialReference {wkid: 4326}
                }
                symbol: stopSymbol
            }

            // Graphic to show the simulated GPS data
            Graphic {
                geometry: polyline
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDashDotDot
                    color: "red"
                    width: 5
                }
            }
        }

        SimpleMarkerSymbol {
            id: stopSymbol
            style: Enums.SimpleMarkerSymbolStyleDiamond
            color: "darkred"
            size: 20
        }

        Stop {
            id: stop1
            geometry: conventionCenterPoint
        }
        Stop {
            id: stop2
            geometry: memorialPoint
        }
        Stop {
            id: stop3
            geometry: aerospaceMuseumPoint
        }

        RouteTask {
            id: routeTask
            url: routeTaskUrl
            Component.onCompleted: {
                load();
            }
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    createDefaultParameters();
                }
            }
            onCreateDefaultParametersStatusChanged: {
                if (createDefaultParametersStatus !== Enums.TaskStatusCompleted) {
                    return;
                }

                createDefaultParametersResult.returnStops = true;
                createDefaultParametersResult.returnDirections = true;
                createDefaultParametersResult.returnRoutes = true;
                createDefaultParametersResult.outputSpatialReference = Factory.SpatialReference.createWgs84();
                createDefaultParametersResult.setStops([stop1, stop2, stop3]);

                //solve the route with these parameters
                routeTask.solveRoute(createDefaultParametersResult);
            }
            onSolveRouteStatusChanged: {
                if (solveRouteStatus === Enums.TaskStatusCompleted) {
                    if (solveRouteResult.routes.length > 0) {
                        m_routeResult = solveRouteResult;
                        m_route = solveRouteResult.routes[0];
                        mapView.setViewpointGeometryAndPadding(m_route.routeGeometry, 100);
                        routeAheadGraphic.geometry = m_route.routeGeometry;
                        navigateButton.enabled = true;
                    }
                }
            }
        }

        locationDisplay.onLocationChanged: {
//            routeTracker.trackRuntimeLocation(locationDisplay.location); // needs to be started if using simulatedLocationDataSource for location display
        }

        // enable "recenter" button
        locationDisplay.onAutoPanModeChanged: {
            recenterButton.enabled = locationDisplay.autoPanMode !== Enums.LocationDisplayAutoPanModeNavigation;
        }

        Rectangle {
            id: backBox
            z: 1
            width: buttonRow.width * 1.5
            height: 200
            color: "#FBFBFB"
            border.color: "black"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 20

            RowLayout {
                id: buttonRow
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    margins: 5
                }
                Button {
                    id: navigateButton
                    text: "Navigate"
                    enabled: false
                    onClicked: {
                        startNavigation();
                        enabled = false;
                    }

                }
                Button {
                    id: recenterButton
                    text: "Recenter"
                    enabled: false
                    onClicked: {
                        recenterMap();
                    }
                }
            }

            Rectangle {
                anchors {
                    top: buttonRow.bottom
                    left: parent.left
                    margins: 5
                }
                width: parent.width
                Text {
                    padding: 5
                    width: parent.width
                    wrapMode: Text.Wrap
                    text: textString
                }
            }
        }

        SimulatedLocationDataSource {
            id: simulatedLocationDataSource
        }

        RouteTrackerLocationDataSource {
            id: routeTrackerLocDataSource
            locationDataSource: simulatedLocationDataSource
            routeTracker: routeTracker
        }

        SimulationParameters {
            id: simulationParameters
            velocity: 40
        }

        RouteTracker {
            id: routeTracker

            onTrackingStatusResultChanged: {
                textString = "Route status: \n";
                if (routeTracker.trackingStatusResult.destinationStatus === Enums.DestinationStatusApproaching || routeTracker.trackingStatusResult.destinationStatus === Enums.DestinationStatusNotReached) {
                    textString += "Distance remaining: " + trackingStatusResult.routeProgress.remainingDistance.displayText + " " +
                            trackingStatusResult.routeProgress.remainingDistance.displayTextUnits.pluralDisplayName + "\n";
                    const time = new Date(trackingStatusResult.routeProgress.remainingTime * 60 * 1000);
                    const hours = time.getUTCHours();
                    const minutes = time.getUTCMinutes();
                    const seconds = time.getSeconds();
                    textString += "Time remaining: " + hours.toString().padStart(2, '0') + ':' + minutes.toString().padStart(2, '0') + ':' +
                            seconds.toString().padStart(2, '0') + "\n";

                    // display next direction
                    if (trackingStatusResult.currentManeuverIndex + 1 < directionListModel.count) {
                        textString += "Next direction: " + directionListModel.get(trackingStatusResult.currentManeuverIndex + 1).directionText;
                    }

                    routeTraveledGraphic.geometry = trackingStatusResult.routeProgress.traversedGeometry;
                    routeAheadGraphic.geometry = trackingStatusResult.routeProgress.remainingGeometry;
                } else if (routeTracker.trackingStatusResult.destinationStatus === Enums.DestinationStatusReached) {
                    textString += "Destination reached.\n";

                    // set the route geometries to reflect the completed route
                    routeTraveledGraphic.geometry = trackingStatusResult.routeResult.routes[0].routeGeometry;

                    // navigate to next stop, if available
                    if (trackingStatusResult.remainingDestinationCount > 1) {
                        switchToNextDestination();
                    } else {
//                        simulatedLocationDataSource.stop();
                    }
                }
            }

            // output new voice guidance
            onNewVoiceGuidanceResultChanged: {
                NavigateRouteSpeaker.textToSpeech(newVoiceGuidanceResult.text);
            }
        }

        // polyline of points to simulated GPS data
        Polyline {
            id: polyline
            json: {"paths":[[[-13042226.3795109,3856415.2300094147],[-13042226.009634402,3856411.9011209253],[-13042197.159267493,3856405.6132204453],[-13042156.102976123,3856384.530260012],[-13042119.485202739,3856353.4606341105],[-13042093.223971322,3856334.9668091694],[-13042062.894098418,3856324.6102672019],[-13042039.961755492,3856295.0201472957],[-13042011.111388583,3856258.7722504106],[-13041974.123738701,3856253.9638559259],[-13041957.109419754,3856217.7159590409],[-13041933.807200328,3856208.4690465704],[-13041919.751893371,3856225.853242015],[-13041943.054112798,3856245.456696453],[-13041970.79485021,3856271.7179278699],[-13042001.124723114,3856298.7189122844],[-13042035.523237506,3856312.7742192396],[-13042064.373604413,3856352.351004614],[-13042112.457549261,3856386.0097660073],[-13042143.897051662,3856403.0240849536],[-13042189.391861018,3856445.5598823186],[-13042251.531112839,3856461.8344482649],[-13042289.998268716,3856505.4798751269],[-13042356.206162006,3856533.5904890378],[-13042389.864923399,3856574.6467804075],[-13042411.317760332,3856595.359864342],[-13042583.310332309,3856698.1855310132],[-13042770.097964225,3856817.6556401323],[-13042865.156224424,3856864.2600789848],[-13042933.213500226,3856898.288716869],[-13042985.366086561,3856935.2763667516],[-13043050.464350356,3857001.854136541],[-13043121.110761631,3857036.9924039296],[-13043205.072726866,3857072.8704243158],[-13043298.651481085,3857063.6235118434],[-13043370.407521857,3857055.4862288693],[-13043395.189247278,3857076.5691893026],[-13043465.465782056,3857076.9390658014],[-13043488.768001482,3857053.2669698764],[-13043535.002563836,3857067.3222768316],[-13043567.181819234,3857081.0077072885],[-13043600.100827638,3857095.0630142419],[-13043587.155150179,3857129.4615286328],[-13043616.745270086,3857167.1889315136],[-13043633.759589031,3857183.0936209629],[-13043640.417366019,3857434.2397636562],[-13043653.732919982,3857638.7814675043],[-13043421.450478718,3857629.164678535],[-13043287.555186158,3857632.8634435227],[-13043206.182356417,3857635.8224555133],[-13043111.493972717,3857629.164678534],[-13043003.120158561,3857611.0407300917],[-13042424.415480388,3857659.9068389949],[-13042234.530311404,3857617.2194754253],[-13042099.84431945,3857613.5395302898],[-13041876.103655247,3857615.0115083433],[-13041603.051726203,3857640.0351352636],[-13041590.539912758,3857891.7433825196],[-13041607.467660381,3858047.0370672313],[-13041572.140187083,3858069.1167380437],[-13041524.300900323,3858039.6771769607],[-13041485.293481888,3858052.1889904211],[-13041502.957218546,3858185.4030043157],[-13041469.1017233,3858316.4090511343],[-13041394.030842556,3858435.639273515],[-13041338.095676498,3858468.0227907058],[-13041261.552817684,3858611.5406509847],[-13041271.120675035,3858654.2280145548],[-13041197.521772355,3858851.4730738038],[-13041141.586606298,3858854.4170299121],[-13041089.331385393,3858981.0071425657],[-13041024.564351011,3859059.7579684621],[-13040828.055280799,3859159.8524761414],[-13040750.776432958,3859263.6269289586],[-13040663.929727774,3859358.5695134471],[-13040659.513793612,3859466.0239113993],[-13040687.48137664,3859470.439845562],[-13040742.680553671,3859501.3513846989],[-13040775.800059889,3859510.9192420505],[-13040772.120114753,3859584.5181447575]]],
            "spatialReference":{"wkid":102100,"latestWkid":3857}}
        }
    }
    function startNavigation() {
        // get the directions for the route
        directionListModel = m_route.directionManeuvers;

        // set properties for route tracker
        routeTracker.routeResult = m_routeResult;
        routeTracker.routeIndex = 0;

        // turn on mapview's navigation mode
        mapView.locationDisplay.autoPanMode = Enums.LocationDisplayAutoPanModeNavigation;

        // add a data source for the location display
        // simulatedLocationDataSource.setLocationsWithPolylineAndParameters(m_route.routeGeometry, simulationParameters);
        simulatedLocationDataSource.setLocationsWithPolylineAndParameters(polyline, simulationParameters);
        routeTrackerLocDataSource.locationDataSource = simulatedLocationDataSource;
        mapView.locationDisplay.dataSource = routeTrackerLocDataSource;
        routeTrackerLocDataSource.start();
    }

    function recenterMap() {
        mapView.locationDisplay.autoPanMode = Enums.LocationDisplayAutoPanModeNavigation;
    }
}
