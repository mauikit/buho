import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.0
import org.kde.mauikit 1.0 as Maui
import org.kde.kirigami 2.7 as Kirigami


Maui.Page
{
    id: control
    property alias editor: _editor
    property string backgroundColor: note.color ? note.color : "transparent"
    property bool showEditActions : false
    property var note : ({})

    signal noteSaved(var note)

    floatingFooter: true

    Maui.Editor
    {
        id: _editor
        fileUrl: control.note.url ? control.note.url : ""
        showLineNumbers: false
        document.autoReload: settings.autoReload
        document.autoSave: settings.autoSave
        anchors.fill: parent

        body.font: settings.font

        autoHideHeader: true
        autoHideHeaderMargins: control.height * 0.3

        Kirigami.Theme.backgroundColor: control.backgroundColor !== "transparent" ? control.backgroundColor : Kirigami.Theme.backgroundColor
        Kirigami.Theme.textColor: control.backgroundColor  !== "transparent" ? Qt.darker(control.backgroundColor, 2) : control.Kirigami.Theme.textColor

        document.enableSyntaxHighlighting: false
        body.placeholderText: qsTr("Title\nBody")
        footBar.visible: false

        headBar.farLeftContent: ToolButton
        {
            icon.name: "go-previous"
            onClicked:
            {
                packNote()
            }
        }

        headBar.rightContent: [

            ToolButton
            {
                id: favButton
                icon.name: "love"
                checkable: true
                checked:  note.favorite == 1
                icon.color: checked ? "#ff007f" : Kirigami.Theme.textColor

            },

            ColorsBar
            {
                onColorPicked: control.backgroundColor = color
                currentColor: control.backgroundColor
            },

            Maui.ToolButtonMenu
            {
                icon.name: "overflow-menu"

                MenuItem
                {
                    text: qsTr("Share")
                    icon.name: "document-share"

                    onTriggered: Maui.Handy.isAndroid ? Maui.Android.shareText(editor.body.text) :
                                                        shareDialog.show(editor.body.text)
                }

                MenuItem
                {
                    text: qsTr("Export")
                    icon.name: "document-export"
                }

                MenuItem
                {
                    text: qsTr("Delete")
                    icon.name: "entry-delete"
                    Kirigami.Theme.textColor: Kirigami.Theme.negativeTextColor
                }
            }
        ]
    }

    footer: Maui.TagsBar
    {
        id: tagBar
        position: ToolBar.Footer
        width: parent.width
        allowEditMode: true
        onTagsEdited:
        {
            if(editor.fileUrl)
                tagBar.list.updateToUrls(tags)
        }

        list.strict: true
        list.urls: editor.fileUrl ? [editor.fileUrl] : []
    }

    function clear()
    {
        editor.body.clear()
        control.note = ({})
    }

    function packNote()
    {
        if(_editor.document.modified /*|| control.note.favorite != favButton.checked */)
        {
            const content =  editor.body.text
            if(content.length > 0)
            {
                var note  = {
                    url: editor.fileUrl,
                    content: content,
                    favorite: favButton.checked ? 1 : 0,
                    format: ".txt" //for now only simple txt files
                }

                if(control.backgroundColor  !== "transparent")
                {
                    note["color"] = control.backgroundColor
                }

                control.noteSaved(note)
            }
        }

        control.clear()
        control.parent.pop(StackView.Immediate)
    }
}
