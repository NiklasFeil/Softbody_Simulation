from dataclasses import dataclass
from math import pi, cos, sin

@dataclass
class V:
    x: float
    y: float
    z: float

    def get_line(self):
        return f"v {self.x} {self.y} {self.z}\n"


@dataclass
class T:
    i: int
    j: int
    k: int

    def get_line(self):
        return f"f {self.i+1} {self.j+1} {self.k+1}\n" # OBJ starts indexing at 1


vertices = []
triangles = []

radius = 1
phi = -pi
theta = -pi/2

x = cos(theta) * sin(phi) * radius
y = sin(theta) * radius
z = cos(theta) * cos(phi) * radius

vertices.append(V(x, y, z))

horizontal_resolution = 15
vertical_resolution = 15 # number of verticle stages including singularity stages

theta += pi/(vertical_resolution-1)

for h in range(0, vertical_resolution-2):
    phi = -pi
    for w in range(0, horizontal_resolution):
        x = cos(theta) * sin(phi) * radius
        y = sin(theta) * radius
        z = cos(theta) * cos(phi) * radius
        vertices.append(V(x, y, z))
        phi += (2 * pi) / horizontal_resolution
        
    theta += pi/(vertical_resolution-1)

x = cos(theta) * sin(phi) * radius
y = sin(theta) * radius
z = cos(theta) * cos(phi) * radius

vertices.append(V(x, y, z))

# Create triangles

def get_index(vertical_index, horizontal_index):
    vertical_index %= vertical_resolution
    horizontal_index %= horizontal_resolution
    if(vertical_index == 0):
        return 0
    if(vertical_index == vertical_resolution-1):
        return 1 + horizontal_resolution * (vertical_resolution-2) # Last index
    return 1 + horizontal_index + horizontal_resolution * (vertical_index-1)

# 1. Connect vertices of stage 2 with the singular vertex in stage 1
for i in range(horizontal_resolution):
    triangle = T(get_index(0, 0), get_index(1, i), get_index(1, i+1))
    triangles.append(triangle)

# 2. Connect vertices of stage i with the vertices of stage i+1
# triangle_1 and triangle_2 form a quad
for i in range(1, vertical_resolution-2):
    for j in range(0, horizontal_resolution):
        triangle_1 = T(get_index(i  , j), get_index(i+1, j  ), get_index(i+1, j+1))
        triangle_2 = T(get_index(i+1, j), get_index(i+1, j+1), get_index(i  , j+1))
        triangles.append(triangle_1)
        triangles.append(triangle_2)

# 3. Connect vertices of almost last stage with vertex at top
for i in range(horizontal_resolution):
    triangle = T(get_index(vertical_resolution-2, i), get_index(vertical_resolution-2, i+1), get_index(vertical_resolution-1, 0))
    triangles.append(triangle)

print(triangles)
print(len(triangles))

with open("detailed_sphere.obj", "w") as file:
    for v in vertices:
        file.write(v.get_line())

    for t in triangles:
        file.write(t.get_line())