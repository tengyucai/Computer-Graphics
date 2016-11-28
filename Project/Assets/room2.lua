grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0)
stone = gr.material({0.9, 0.9, 0.9}, {0.0, 0.0, 0.0}, 0, 0, 0)
red = gr.material({0.9, 0.1, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0)
blue = gr.material({0.2, 0.2, 0.9}, {0.0, 0.0, 0.0}, 0, 0, 0)
mat3 = gr.material({0.2, 0.2, 0.2}, {0.2, 0.2, 0.2}, 25, 0.8, 1.85) -- glass
mat4 = gr.material({0.35, 0.35, 0.4}, {0.6, 0.7, 0.6}, 25, 0, 0)
mat1 = gr.material({0.05, 0.05, 0.15}, {0.01, 0.01, 0.05}, 25, 0.95, 1) -- water

scene = gr.node('root')

back = gr.cube('back')
scene:add_child(back)
back:set_material(stone)
back:translate(-0.5, -0.5, -1.5)

buttom = gr.cube('buttom')
scene:add_child(buttom)
buttom:set_material(grass)
buttom:translate(-0.5, -1.5, -0.5)

top = gr.cube('top')
scene:add_child(top)
top:set_material(stone)
top:translate(-0.5, 0.5, -0.5)

right = gr.cube('right')
scene:add_child(right)
right:set_material(blue)
right:translate(0.5, -0.5, -0.5)

left = gr.cube('left')
scene:add_child(left)
left:set_material(red)
left:translate(-1.5, -0.5, -0.5)

s1 = gr.nh_sphere('s1', {0.3, -0.35, 0.10}, 0.15)
-- s1 = gr.nh_sphere('s1', {0, 0, 0}, 0.15)
scene:add_child(s1)
s1:set_material(mat3)

s2 = gr.nh_sphere('s2', {-0.25, -0.35, -0.25}, 0.15)
-- s1 = gr.nh_sphere('s1', {0, 0, 0}, 0.15)
scene:add_child(s2)
s2:set_material(mat4)

-- water = gr.mesh('water', 'Assets/plane.obj' )
-- scene:add_child(water)
-- water:scale(2, 2, 2)
-- water:translate(0, -0.15, 0)
-- water:set_material(mat1)

-- water = gr.mesh('water', 'Assets/water.obj' )
-- scene:add_child(water)
-- water:scale(1,1,1)
-- water:rotate('X', 10)
-- water:set_material(mat3)

-- cube = gr.cube('cube')
-- cube:set_material(mat3)
-- cube:scale(0.25, 0.25, 0.25)

-- csg = gr.union('difference', s1, cube)
-- csg:translate(0.2, -0.3, -0.15)
-- scene:add_child(csg)


white_light = gr.light({0, 0.48, 0.0}, {0.6, 0.6, 0.6}, {1, 0, 0}, 10000)

gr.render(scene, 'test.png', 256, 256,
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
