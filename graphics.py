import vtk
import numpy as np
from basic_math import Vector, Point
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
    line_source.SetPoint1(pt1.x, pt1.y, pt1.z)
    line_source.SetPoint2(pt2.x, pt2.y, pt2.z)

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
    sphere_source.SetCenter(pt.x, pt.y, pt.z)
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
    # Init data
    s = Point(-2, 0, 0)  # источник света
    started = Point(0, 0, 0)
    r1 = 3
    # r2 = 4
    d0 = .1
    h_lense = .2
    phi = 0
    theta = 0.1

    n1 = 1
    n2 = 1.67
    #

    dir_d = Vector(np.cos(phi) * np.sin(theta),
                   np.sin(phi) * np.sin(theta),
                   np.cos(theta))

    b = started.x - d0 * 0.5
    c = r1 ** 2 - (h_lense ** 2) * 0.25

    sphere_centre = Point(-b + 2 * np.sqrt(c),
                          started.y,
                          started.z)  # found

    print(f'Координаты центра сферы для первой поверхности линзы: x - '
          f'{sphere_centre.x}, y - {sphere_centre.y}, z - {sphere_centre.z}')

    f = Vector(sphere_centre.x - s.x,
               sphere_centre.y - s.y,
               sphere_centre.z - s.z)

    aux = ((dir_d * f) ** 2 - (f * f) ** 2 + r1 ** 2)

    t = np.minimum(dir_d * f - np.sqrt(aux), dir_d * f + np.sqrt(aux))

    intersection_pt = Point(s.x + t * dir_d.x,
                            s.y + t * dir_d.y,
                            s.z + t * dir_d.z)  # found
    print(f'Координаты точки пересечения: x - {intersection_pt.x} , y - {intersection_pt.y}, z - {intersection_pt.z}')

    a = intersection_pt.x - sphere_centre.x
    b = intersection_pt.y
    c = intersection_pt.z
    d = dir_d.x
    e = dir_d.y
    f = dir_d.z

    aux = Vector(a, intersection_pt.y, intersection_pt.z)
    normal_v = Vector(aux.x / aux.len(), aux.y / aux.len(), aux.z / aux.len())

    g = -1 + np.sqrt(n1 * (1 - (normal_v * dir_d) ** 2) / n2)
    h = np.cos(phi)

    x = ((c * d - a * f) * (g * b * f - c * b * h) + b * (a * e - b * d) * (b * h - e * g)) / (
            -(c * d - a * f) * (a * b * f + c * b * d) - b * (a * e - b * d) ** 2)
    y = 1 / b * (g - a * x - c * ((b * h - e * g) / (b * f - c * e) + x * (a * e - b * d) / (b * f - c * e)))
    z = (a * h - d * g) / (c * d - a * f) + y * (b * d - a * e) / (c * d - a * f)

    print(f'Координаты направляющего вектора преломленного луча : x - {x}, y - {y}, z - {z}')
    l = Vector(3 * x, 3 * y, 3 * z)

    refracted_pt = Point(intersection_pt.x + l.x,
                         intersection_pt.y + l.y,
                         intersection_pt.z + l.z)

    # Create two points, P0 and P1

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

    ren.AddActor(create_sphere(sphere_centre, r1))
    ren.AddActor(create_line(s, intersection_pt))
    ren.AddActor(create_line(intersection_pt, refracted_pt))
    ren.SetBackground(colors.GetColor3d("DarkGreen"))

    interactor.Initialize()
    rw.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
