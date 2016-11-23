grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0)
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 0)
red = gr.material({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.material({0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, 0, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 0, 0.4)

scene = gr.node('root')

-- the floor
back = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(back)
back:set_material(stone)
back:rotate('X', 90)
back:scale(260, 260, 260)

buttom = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(buttom)
buttom:set_material(grass)
buttom:scale(260, 260, 260)
buttom:translate(0, -260, 0)

top = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(top)
top:set_material(grass)
top:rotate('X', 180)
top:scale(260, 260, 260)
top:translate(0, 260, 0)

right = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(right)
right:set_material(blue)
right:rotate('Z', 90)
right:scale(260, 260, 260)
right:translate(260, 0, 0)

left = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(left)
left:set_material(red)
left:rotate('Z', -90)
left:scale(260, 260, 260)
left:translate(-260, 0, 0)

s1 = gr.nh_sphere('s1', {0, -50, 200}, 100)
scene:add_child(s1)
s1:set_material(mat3)

white_light = gr.light({0, 100, 400}, {0.6, 0.6, 0.6}, {1, 0, 0}, 10000)
white_light2 = gr.light({100.0, -100.0, 150.0}, {0.9, 0.9, 0.9}, {1, 0, 0}, 10000)

gr.render(scene, 'test.png', 256, 256,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
