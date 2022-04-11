import vtk
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballActor
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkFiltersSources import vtkLineSource

from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkProperty,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)

colors = vtkNamedColors()


class MyInteractorStyle(vtkInteractorStyleTrackballCamera):

    def __init__(self, parent=None):
        self.AddObserver('MiddleButtonPressEvent', self.middle_button_press_event)
        self.AddObserver('MiddleButtonReleaseEvent', self.middle_button_release_event)

    def middle_button_press_event(self, obj, event):
        print('Middle Button pressed')
        self.OnMiddleButtonDown()
        return

    def middle_button_release_event(self, obj, event):
        print('Middle Button released')
        self.OnMiddleButtonUp()
        return


def create_line(pt1, pt2):
    line_source = vtkLineSource()
    line_source.SetPoint1(pt1)
    line_source.SetPoint2(pt2)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(line_source.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetLineWidth(4)
    actor.GetProperty().SetColor(colors.GetColor3d("Peacock"))

    return actor


def create_sphere(pt, r):
    # Create a sphere
    sphere_source = vtkSphereSource()
    sphere_source.SetCenter(pt)
    sphere_source.SetRadius(r)
    # Make the surface smooth.
    sphere_source.SetPhiResolution(20)

    sphere_source.SetThetaResolution(20)
    sphere_source.SetStartPhi(0)  # тут задаем угол линзы
    sphere_source.SetEndPhi(180)
    # sphere_source.SetStartTheta(-45)
    # sphere_source.SetEndTheta(45)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(sphere_source.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d("White"))
    actor.GetProperty().SetRepresentationToWireframe()

    return actor


def main():
    # Create two points, P0 and P1
    p0 = [0.0, 0.0, 0.0]
    p1 = [2.0, 2.0, 2.0]

    # Visualize

    ren = vtkRenderer()
    rw = vtkRenderWindow()
    rw.SetWindowName("Sphere")
    rw.AddRenderer(ren)
    rwi = vtkRenderWindowInteractor()
    rwi.SetRenderWindow(rw)

    # style = vtkInteractorStyleTrackballActor()
    # rwi.SetInteractorStyle(style)

    interactor = vtkRenderWindowInteractor()
    interactor.SetInteractorStyle(MyInteractorStyle())
    interactor.SetRenderWindow(rw)

    ren.AddActor(create_sphere([0.0, 0.0, 0.0], 2))
    ren.AddActor(create_line(p0, p1))
    ren.SetBackground(colors.GetColor3d("DarkGreen"))

    interactor.Initialize()
    rw.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
