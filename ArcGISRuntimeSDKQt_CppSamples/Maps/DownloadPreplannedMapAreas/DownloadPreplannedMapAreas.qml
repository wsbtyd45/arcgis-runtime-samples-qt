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


import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

DownloadPreplannedMapAreasSample {
    id: rootRectangle

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    clip: true
    width: 800
    height: 600

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Rectangle {
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            margins: 40 * scaleFactor
        }
        width: parent.width * 0.35
        height: childrenRect.height
        color: "white"

        Column
        {
            width: parent.width
            height: childrenRect.height


            Text {
                text: "Select area for download:"
            }

            ListView {
                id: areaSelectionView
                clip: true
                width: parent.width
                height: childrenRect.height

                spacing: 5 * scaleFactor

                model: areaList

                delegate: Item {
                    width: childrenRect.width
                    height: childrenRect.height
                    Image {
                        id: areaImage
                        width: 44 * scaleFactor
                        height: width
                        fillMode: Image.PreserveAspectFit
                        source: itemThumbnailUrl
                        anchors.margins: 5 * scaleFactor
                    }

                    Text {
                        id: areaTitle
                        anchors.left: areaImage.right
                        anchors.top: areaImage.top
                        anchors.margins: 5 * scaleFactor
                        text: itemTitle

                    }

                    Text {
                        anchors.left: areaImage.right
                        anchors.top: areaTitle.bottom
                        anchors.margins: 5 * scaleFactor
                        width: areaSelectionView.width - areaImage.width
                        text: itemDescription
                        wrapMode: Text.Wrap
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: areaSelectionView.currentIndex = index;

                    }
                }

                highlightFollowsCurrentItem: true
                highlight: Rectangle {
                    height: areaSelectionView.currentItem.height
                    color: "lightsteelblue"
                }
            }

            Row
            {
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    anchors.margins: 5 * scaleFactor
                    id: downloadButton
                    text: "Download"
                    onClicked: {
                        var index = areaSelectionView.currentIndex;
                        var path = "OfflineMap_%1.mmpk".arg(index);
                        rootRectangle.downloadPreplannedMapArea(index, path);
                    }
                }

                Button {
                    anchors.margins: 5 * scaleFactor
                    id: clearButton
                    text: "Delete all offline areas"
                    onClicked: {
                        clearAllDownloadedMaps();
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: 135 * scaleFactor
        height: 100 * scaleFactor
        color: "lightgrey"
        opacity: 0.8
        radius: 5
        visible: indicator.running
        border {
            color: "#4D4D4D"
            width: 1 * scaleFactor
        }

        Column {
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            spacing: 10

            BusyIndicator {
                id: indicator
                anchors.horizontalCenter: parent.horizontalCenter
                running: jobProgress > 0 && jobProgress < 100
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Loading: %1%".arg(jobProgress)
                font.pixelSize: 16 * scaleFactor
            }
        }
    }

    MessageDialog {
        id: messageDialog
    }

    onJobFailed: {
        messageDialog.text = message + '\n' + details;
        messageDialog.open();
    }

}
