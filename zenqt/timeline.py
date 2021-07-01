from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtSvg import *

from . import asset_path

import zenvis


class QDMPlayButton(QSvgWidget):
    def __init__(self, timeline):
        super().__init__()
        self.render = self.renderer()
        self.load(asset_path('stop.svg'))
        self.timeline = timeline
        self.checked = True
        # PyQt5 >= 5.15
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)
    
    def isChecked(self):
        return self.checked
    
    def mousePressEvent(self, event):
        super().mouseMoveEvent(event)
        self.checked = not self.checked
        self.timeline.value_changed()
        if self.checked:
            self.load(asset_path('stop.svg'))
        else:
            self.load(asset_path('play.svg'))
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)

class QDMNextButton(QSvgWidget):
    def __init__(self, timeline):
        super().__init__()
        self.render = self.renderer()
        self.load(asset_path('next.svg'))
        self.timeline = timeline
        # PyQt5 >= 5.15
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)
    
    def mousePressEvent(self, event):
        super().mouseMoveEvent(event)
        self.timeline.next_frame()
        self.load(asset_path('next-click.svg'))
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)
    
    def mouseReleaseEvent(self, event):
        self.load(asset_path('next.svg'))
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)

class QDMPrevButton(QSvgWidget):
    def __init__(self, timeline):
        super().__init__()
        self.render = self.renderer()
        self.load(asset_path('prev.svg'))
        self.timeline = timeline
        # PyQt5 >= 5.15
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)
    
    def mousePressEvent(self, event):
        super().mouseMoveEvent(event)
        self.timeline.prev_frame()
        self.load(asset_path('prev-click.svg'))
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)
    
    def mouseReleaseEvent(self, event):
        self.load(asset_path('prev.svg'))
        self.render.setAspectRatioMode(Qt.KeepAspectRatio)

class TimelineWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.label = QLabel('-')
        self.status = QLabel('-')

        self.slider = QSlider(Qt.Horizontal)
        self.slider.valueChanged.connect(self.value_changed)
        self.slider.setMinimum(0)
        self.slider.setMaximum(1)

        self.player = QDMPlayButton(self)
        self.prev = QDMPrevButton(self)
        self._next = QDMNextButton(self)

        layout = QHBoxLayout()
        layout.addWidget(self.player)
        layout.addWidget(self.prev)
        layout.addWidget(self._next)
        layout.addWidget(self.label)
        layout.addWidget(self.slider)
        layout.addWidget(self.status)
        self.setLayout(layout)
        self.setFixedHeight(38)

    def setEditor(self, editor):
        self.editor = editor
        self.maxframe = self.editor.edit_nframes
        self.maxframe.textChanged.connect(self.maxframe_changed)
        self.maxframe.setText(str(self.slider.maximum()))

    def maxframe_changed(self):
        self.slider.setMaximum(int('0' + self.maxframe.text()))

    def on_update(self):
        frameid = zenvis.status['frameid']
        self.slider.setValue(frameid)
        self.label.setText(str(frameid))
        self.status.setText(self.get_status_string())

    def value_changed(self):
        zenvis.status['next_frameid'] = self.slider.value()
        zenvis.status['playing'] = self.player.isChecked()

    def get_status_string(self):
        fps = zenvis.status['render_fps']
        spf = zenvis.status['solver_interval']
        return f'{fps:.1f} FPS | {spf:.02f} secs/step'

    def next_frame(self):
        # TODO
        print('next')

    def prev_frame(self):
        # TODO
        print('prev')
