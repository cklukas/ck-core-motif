---
nav_exclude: true
---
# Container cell geometry

Grid and cell layouts derive their cell counts by subtracting twice the
container margins from unsigned widget dimensions. A container narrower than
its margins could wrap those values before division, producing a huge cell
count and unnecessary placement work.

The layout now performs the subtraction in signed arithmetic and exits when
either available dimension cannot contain a cell. Normal grids keep their
existing counts while collapsed containers remain stable.
