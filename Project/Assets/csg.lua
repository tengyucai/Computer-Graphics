-- -- test for hierarchical ray-tracers.
-- -- Thomas Pflaum 1996

-- gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0, 0)
-- grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0)
-- blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 0, 0)

-- scene = gr.node('scene')
-- -- scene:rotate('X', 23)
-- -- scene:translate(6, -2, -15)

-- sphere = gr.sphere('sphere')
-- sphere:set_material(gold)
-- cube = gr.cube('cube')
-- cube:set_material(blue)
-- cube:translate(-0.5,-0.5,-0.5)
-- cube:scale(1.7,1.7,1.7)

-- csg1 = gr.union('union', sphere, cube)
-- scene:add_child(csg1)
-- csg1:rotate('Y', 60)
-- csg1:rotate('X', 30)
-- csg1:translate(-2, -2, 0)

-- csg2 = gr.intersection('intersection', sphere, cube)
-- scene:add_child(csg2)
-- csg2:rotate('Y', 60)
-- csg2:rotate('X', 30)
-- csg2:translate(-2, 2, 0)

-- csg3 = gr.difference('difference', cube, sphere)
-- scene:add_child(csg3)
-- csg3:rotate('Y', 60)
-- csg3:rotate('X', 30)
-- csg3:translate(2, 2, 0)

-- cylinder1 = gr.cylinder('cylinder')
-- cylinder1:set_material(gold)

-- cylinder2 = gr.cylinder('cylinder')
-- cylinder2:set_material(blue)
-- cylinder2:scale(0.95, 0.95, 0.95)
-- cylinder2:translate(0, 0, 0.2)

-- csg4 = gr.difference('difference', cylinder1, cylinder2)
-- scene:add_child(csg4)
-- csg4:rotate('X', -60)
-- csg4:translate(2, -2, 0)

-- -- the floor
-- plane = gr.mesh( 'plane', 'Assets/plane.obj' )
-- scene:add_child(plane)
-- plane:set_material(grass)
-- plane:rotate('X', 90)
-- plane:scale(30, 30, 30)
-- plane:translate(0, 0, -2)

-- -- The lights
-- l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
-- l2 = gr.light({0, 5, 220}, {0.4, 0.4, 0.8}, {1, 0, 0})

-- gr.render(scene, 'csg.png', 256, 256, 
-- 	  {0, 0, 6}, {0, 0, -1}, {0, 1, 0}, 80,
-- 	  {0.4, 0.4, 0.4}, {l1, l2})


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

cylinder1 = gr.cylinder('cylinder')
cylinder1:set_material(red)

cylinder2 = gr.cylinder('cylinder')
cylinder2:set_material(red)
cylinder2:scale(0.97, 0.97, 1.1)

ring = gr.difference('difference', cylinder1, cylinder2)
scene:add_child(ring)
ring:rotate('X', 30)
ring:scale(0.1, 0.1, 0.1)
ring:translate(0, 0, 0)

white_light = gr.light({0, 0.5, 0.0}, {0.75, 0.75, 0.75}, {1, 0, 0}, 3, 1000000)

gr.render(scene, 'csg.png', 256, 256,
	  {0, 0, 1.5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
