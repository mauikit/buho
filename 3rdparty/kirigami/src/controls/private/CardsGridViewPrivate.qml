/*
 *   Copyright 2018 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.6
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami


GridView {
    id: root

    property Component _delegateComponent
    delegate: Kirigami.DelegateRecycler {
        width: Math.min(root.cellWidth, root.maximumColumnWidth) - Kirigami.Units.largeSpacing * 2

        //in grid views align the cells in the middle
        anchors.left: parent.left
        anchors.leftMargin: root.width <= root.maximumColumnWidth ? 0 : (index % 2 === 0 ? Math.max(0, root.cellWidth - root.maximumColumnWidth) : root.cellWidth)

        sourceComponent: root._delegateComponent
    }
}
