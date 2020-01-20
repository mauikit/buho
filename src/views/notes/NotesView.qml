import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.kde.mauikit 1.0 as Maui
import org.kde.kirigami 2.7 as Kirigami
import Notes 1.0
import Qt.labs.platform 1.0

import "../../widgets"

Maui.Page
{
    property var currentNote : ({})

    property alias cardsView : cardsView
    property alias model : notesModel
    property alias list : notesList
    property alias currentIndex : cardsView.currentIndex

    signal noteClicked(var note)

    padding: Maui.Style.space.big

    headBar.visible: !cardsView.holder.visible

    headBar.leftContent: [
        ToolButton
        {
            icon.name:  cardsView.gridView ? "view-list-details" : "view-list-icons"
            onClicked:
            {
                cardsView.gridView = !cardsView.gridView
            }
        }
    ]

    headBar.middleContent: Maui.TextField
    {
        Layout.fillWidth: true
        placeholderText: cardsView.count + " " + qsTr("notes")
        onAccepted: notesModel.filter = text
        onCleared: notesModel.filter = ""
    }

    headBar.rightContent: [
        ToolButton
        {
            icon.name: "view-sort"
            onClicked: sortMenu.open();

            Menu
            {
                id: sortMenu

                MenuItemGroup
                {
                    id: orderGroup
                }

                MenuItemGroup
                {
                    id: sortGroup
                }

                MenuItem
                {
                    text: qsTr("Ascedent")
                    checkable: true
                    checked: notesList.order === Notes.ASC
                    onTriggered: notesList.order = Notes.ASC
                    group: sortGroup
                }

                MenuItem
                {
                    text: qsTr("Descendent")
                    checkable: true
                    checked: notesList.order === Notes.DESC
                    onTriggered: notesList.order = Notes.DESC
                    group: sortGroup
                }

                MenuSeparator{}

                MenuItem
                {
                    text: qsTr("Title")
                    checkable: true
                    checked: notesList.sortBy === Notes.TITLE
                    onTriggered: notesList.sortBy = Notes.TITLE
                    group: orderGroup
                }

                MenuItem
                {
                    text: qsTr("Color")
                    checkable: true
                    checked: notesList.sortBy === Notes.COLOR
                    onTriggered: notesList.sortBy = Notes.COLOR
                    group: orderGroup
                }

                MenuItem
                {
                    text: qsTr("Add date")
                    checkable: true
                    checked: notesList.sortBy === Notes.ADDDATE
                    onTriggered: notesList.sortBy = Notes.ADDDATE
                    group: orderGroup
                }

                MenuItem
                {
                    text: qsTr("Updated")
                    checkable: true
                    checked: notesList.sortBy === Notes.Modified
                    onTriggered: notesList.sortBy = Notes.Modified
                    group: orderGroup
                }

                MenuItem
                {
                    text: qsTr("Favorite")
                    checkable: true
                    checked: notesList.sortBy === Notes.FAVORITE
                    onTriggered: notesList.sortBy = Notes.FAVORITE
                    group: orderGroup
                }
            }
        },
        ToolButton
        {
            id: pinButton
            icon.name: "pin"
            checkable: true
            icon.color: checked ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }
    ]

    Notes
    {
        id: notesList
    }

    Maui.BaseModel
    {
        id: notesModel
        list: notesList
        recursiveFilteringEnabled: true
        sortCaseSensitivity: Qt.CaseInsensitive
        filterCaseSensitivity: Qt.CaseInsensitive
    }

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        Rectangle
        {
            visible: pinButton.checked
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            height: cardsView.itemHeight
            color: Kirigami.Theme.backgroundColor

            Maui.Holder
            {
                id: holder
                visible: pinnedList.count == 0
                emoji: "qrc:/edit-pin.svg"
                emojiSize: Maui.Style.iconSizes.big
                isMask: true
                title : qsTr("No pins!")
                body: qsTr("No matched pinned notes. You can pin your notes to access them quickly")
                z: 999
           }

            CardsList
            {
                id: pinnedList
                height: parent.height *0.9
                width: parent.width * 0.9
                anchors.centerIn: parent
                itemHeight: cardsView.itemHeight * 0.9
                itemWidth: itemHeight * 1.5
                onItemClicked: noteClicked(cardsView.model.get(index))
            }
        }


        CardsView
        {
            id: cardsView
            Layout.fillHeight: true
            Layout.fillWidth: true
            width: parent.width
            holder.emoji: "qrc:/view-notes.svg"
            holder.emojiSize: Maui.Style.iconSizes.huge
            holder.title :qsTr("No notes!")
            holder.body: qsTr("Click here to create a new note")

            model: notesModel
            delegate: CardDelegate
            {
                id: delegate
                width: Math.min(cardsView.cellWidth, cardsView.itemWidth) - Kirigami.Units.largeSpacing * 2
                height: cardsView.itemHeight
                anchors.left: parent.left
                anchors.leftMargin: cardsView.width <= cardsView.itemWidth ? 0 : (index % 2 === 0 ? Math.max(0, cardsView.cellWidth - cardsView.itemWidth) :
                                                                                                    cardsView.cellWidth)

                onClicked:
                {
                    currentIndex = index
                    currentNote = notesList.get(index)
                    noteClicked(currentNote)
                }

                onRightClicked:
                {
                    currentIndex = index
                    currentNote = notesList.get(index)
                    cardsView.menu.popup()
                }

                onPressAndHold:
                {
                    currentIndex = index
                    currentNote = notesList.get(index)
                    cardsView.menu.popup()
                }
            }

            Connections
            {
                target: cardsView.holder
                onActionTriggered: newNote()
            }

            Connections
            {
                target: cardsView.menu
                onOpened:
                {
                    cardsView.menu.isFav =  currentNote.favorite == 1
                    cardsView.menu.isPin = currentNote.pin == 1
                }

                onDeleteClicked: notesList.remove(cardsView.currentIndex)
                onColorClicked:
                {
                    notesList.update(({"color": color}), cardsView.currentIndex)
                }

                onFavClicked:
                {
                    notesList.update(({"favorite": favorite}), cardsView.currentIndex)
                }

                onPinClicked:
                {
                    notesList.update(({"pin": pin}), cardsView.currentIndex)
                }

                onCopyClicked:
                {
                    Maui.Handy.copyToClipboard(currentNote.content)
                }
            }
        }
    }
}
