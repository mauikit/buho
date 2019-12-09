import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import org.kde.mauikit 1.0 as Maui
import org.kde.kirigami 2.7 as Kirigami

Maui.Dialog
{
    parent: parent
    heightHint: 0.95
    widthHint: 0.95
    maxHeight: previewReady ? Maui.Style.unit * 800 : contentLayout.implicitHeight
    maxWidth: Maui.Style.unit *700

    signal linkSaved(var link)
    property string selectedColor : "#ffffe6"
    property string fgColor: Qt.darker(selectedColor, 2.5)

    property bool previewReady : false
    x: (parent.width / 2) - (width / 2)
    y: (parent.height /2 ) - (height / 2)
    modal: true
    padding: isAndroid ? 1 : undefined
    page.padding: 0

    Connections
    {
        target: linker
        onPreviewReady:
        {
            previewReady = true
            fill(link)
        }
    }
    headBar.visible: previewReady
    footBar.visible: previewReady

    headBar.leftContent: [
        ToolButton
        {
            id: pinButton
            icon.name: "window-pin"
            checkable: true
            icon.color: checked ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
            //                onClicked: checked = !checked
        },

        TextField
        {
            id: title
            visible: previewReady
            Layout.fillWidth: true
            Layout.margins: Maui.Style.space.medium
            height: 24
            placeholderText: qsTr("Title")
            font.weight: Font.Bold
            font.bold: true
            font.pointSize: Maui.Style.fontSizes.large
            color: fgColor

            background: Rectangle
            {
                color: "transparent"
            }
        }
    ]

    headBar.rightContent: ColorsBar
    {
        onColorPicked: selectedColor = color
    }

    footBar.leftContent: [

        ToolButton
        {
            id: favButton
            icon.name: "love"
            checkable: true
            icon.color: checked ? "#ff007f" : Kirigami.Theme.textColor
        },

        ToolButton
        {
            icon.name: "document-share"
            onClicked: isAndroid ? Maui.Android.shareText(link.text) :
                                   shareDialog.show(link.text)
        },

        ToolButton
        {
            icon.name: "document-export"
        }
    ]

    acceptText: qsTr("Save")
    rejectText:  qsTr("Discard")

    onAccepted:
    {
        packLink()
        clear()
    }
    onRejected:  clear()

    ColumnLayout
    {
        id: contentLayout
        anchors.fill: parent

        TextField
        {
            id: link
            Layout.fillWidth: true
            Layout.margins: Maui.Style.space.medium
            height: Maui.Style.rowHeight
            verticalAlignment: Qt.AlignVCenter
            placeholderText: qsTr("URL")
            font.weight: Font.Bold
            font.bold: true
            font.pointSize: Maui.Style.fontSizes.large
            color: fgColor
            Layout.alignment: Qt.AlignCenter

            background: Rectangle
            {
                color: "transparent"
            }

            onAccepted: linker.extract(link.text)
        }

        Item
        {
            visible: previewReady

            Layout.fillWidth: previewReady
            Layout.fillHeight: previewReady

            ListView
            {
                id: previewList
                anchors.fill: parent
                anchors.centerIn: parent
                visible: count > 0
                clip: true
                snapMode: ListView.SnapOneItem
                orientation: ListView.Horizontal
                interactive: count > 1
                highlightFollowsCurrentItem: true
                model: ListModel{}
                onMovementEnded:
                {
                    var index = indexAt(contentX, contentY)
                    currentIndex = index
                }
                delegate: ItemDelegate
                {
                    height: previewList.height
                    width: previewList.width

                    background: Rectangle
                    {
                        color: "transparent"
                    }

                    Image
                    {
                        id: img
                        source: model.url
                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                        width: parent.width
                        height: parent.height
                        sourceSize.height: height
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                    }
                }
            }
        }

        Maui.TagsBar
        {
            id: tagBar
            visible: previewReady
            Layout.fillWidth: visible
            allowEditMode: true
            list.abstract: true
            list.key: "links"
            onTagsEdited: list.updateToAbstract(tags)
            onTagRemovedClicked: list.removeFromAbstract(index)
        }
    }

    function clear()
    {
        title.clear()
        link.clear()
        previewList.model.clear()
        tagBar.clear()
        previewReady = false
        close()

    }

    function fill(link)
    {
        title.text = link.title
        populatePreviews(link.img)
        tagBar.list.lot= link.url

        open()
    }

    function populatePreviews(imgs)
    {
        for(var i in imgs)
        {
            console.log("PREVIEW:", imgs[i])
            previewList.model.append({url : imgs[i]})
        }
    }

    function packLink()
    {
        var data = ({
                        url : link.text,
                        title: title.text,
                        preview: previewList.count > 0 ? previewList.model.get(previewList.currentIndex).url :  "",
                        color: selectedColor,
                        tag: tagBar.getTags(),
                        pin: pinButton.checked,
                        favorite: favButton.checked
                    })
        linkSaved(data)
    }
}
