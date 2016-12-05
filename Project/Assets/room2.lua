brick = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 0, 'Assets/brick.png', 'Assets/brick_norm.png')
dirt = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 0, 'Assets/dirt.png', 'Assets/dirt_norm.png')
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 0)
stone = gr.material({0.9, 0.9, 0.9}, {0.0, 0.0, 0.0}, 0, 0, 0, 0)
red = gr.material({0.9, 0.1, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 0)
blue = gr.material({0.2, 0.2, 0.9}, {0.0, 0.0, 0.0}, 0, 0, 0, 0)
mat3 = gr.material({0.2, 0.2, 0.2}, {0.05, 0.05, 0.05}, 25, 0.8, 1.85, 0) -- glass
mat4 = gr.material({0.35, 0.35, 0.4}, {0.6, 0.7, 0.6}, 25, 0, 0, 0)
mat1 = gr.material({0.2, 0.2, 0.35}, {0.7, 0.7, 0.7}, 25, 0.7, 1.33, 1.05) -- water
air = gr.material({0.6, 0.6, 0.8}, {0, 0, 0}, 25, 0.8, 1.0, 0) -- air
metal = gr.material({0.65, 0.3, 0.3}, {0.3, 0.3, 0.3}, 50, 0, 0, 0)

scene = gr.node('root')

back = gr.cube('back')
scene:add_child(back)
back:set_material(stone)
back:translate(-0.5, -0.5, -1.5)

buttom = gr.cube('buttom')
scene:add_child(buttom)
buttom:set_material(dirt)
buttom:translate(-0.5, -1.5, -0.5)

top = gr.cube('top')
scene:add_child(top)
top:set_material(stone)
top:translate(-0.5, 0.5, -0.5)

right = gr.cube('right')
scene:add_child(right)
right:set_material(brick)
right:translate(0.5, -0.5, -0.5)

left = gr.cube('left')
scene:add_child(left)
left:set_material(brick)
left:translate(-1.5, -0.5, -0.5)

cylinder1 = gr.cylinder('cylinder')
cylinder1:set_material(metal)

cylinder2 = gr.cylinder('cylinder')
cylinder2:set_material(metal)
cylinder2:scale(0.97, 0.97, 1.1)

ring = gr.difference('difference', cylinder1, cylinder2)
scene:add_child(ring)
ring:rotate('X', 90)
ring:scale(0.1, 0.017, 0.1)
ring:translate(-0.1, -0.481, 0.27)

s1 = gr.nh_sphere('s1', {0.3, -0.35, 0.15}, 0.15)
-- s1 = gr.nh_sphere('s1', {0, 0, 0}, 0.15)
scene:add_child(s1)
s1:set_material(mat3)

s2 = gr.nh_sphere('s2', {-0.25, -0.35, -0.25}, 0.15)
-- s1 = gr.nh_sphere('s1', {0, 0, 0}, 0.15)
scene:add_child(s2)
s2:set_material(mat4)

-- -- s3 = gr.nh_sphere('s3', {-0.1, -0.25, 0.2}, 0.02)
-- -- -- s1 = gr.nh_sphere('s1', {0, 0, 0}, 0.15)
-- -- scene:add_child(s3)
-- -- s3:set_material(air)

water = gr.mesh('water', 'Assets/untitled.obj' )
scene:add_child(water)
water:translate(0, -0.1, 0)
water:set_material(mat1)

-- water = gr.mesh('water', 'Assets/water.obj' )
-- scene:add_child(water)
-- water:scale(1.5,1.5,2)
-- water:translate(0, -0.1, 0)
-- water:set_material(mat1)

-- cube = gr.cube('cube')
-- cube:set_material(mat3)
-- cube:scale(0.25, 0.25, 0.25)

-- csg = gr.union('difference', s1, cube)
-- csg:translate(0.2, -0.3, -0.15)
-- scene:add_child(csg)


white_light = gr.light({0, 0.5, 0.0}, {0.7, 0.5, 0.9}, {1, 0, 0}, 10000)

gr.render(scene, 'test.png', 1024, 1024,
	  {0, 0, 1.5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})


-- grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0)
-- stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 0, 0)
-- red = gr.material({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0, 0, 0)
-- blue = gr.material({0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, 0, 0, 0)
-- mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 0, 0.5, 1.5)

-- scene = gr.node('root')

-- back = gr.cube('back')
-- scene:add_child(back)
-- back:set_material(stone)
-- back:translate(-0.5, -0.5, -1.5)

-- buttom = gr.cube('buttom')
-- scene:add_child(buttom)
-- buttom:set_material(grass)
-- buttom:translate(-0.5, -1.5, -0.5)

-- top = gr.cube('top')
-- scene:add_child(top)
-- top:set_material(grass)
-- top:translate(-0.5, 0.5, -0.5)

-- right = gr.cube('right')
-- scene:add_child(right)
-- right:set_material(blue)
-- right:translate(0.5, -0.5, -0.5)

-- left = gr.cube('left')
-- scene:add_child(left)
-- left:set_material(red)
-- left:translate(-1.5, -0.5, -0.5)

-- water = gr.mesh('water', 'Assets/water.obj' )
-- scene:add_child(water)
-- water:scale(1,1,1)
-- water:rotate('X', 10)
-- water:set_material(mat3)

-- -- s1 = gr.nh_sphere('s1', {0, -0.2, 0}, 0.15)
-- -- scene:add_child(s1)
-- -- s1:set_material(mat3)

-- white_light = gr.light({0, 0.48, 0}, {0.6, 0.6, 0.6}, {1, 0, 0}, 10000)

-- gr.render(scene, 'room.png', 128, 128,
-- 	  {0, 0, 1.5}, {0, 0, -1}, {0, 1, 0}, 50,
-- 	  {0.3, 0.3, 0.3}, {white_light})
