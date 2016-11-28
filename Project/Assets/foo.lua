grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0)
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 0, 0)
red = gr.material({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0, 0, 0)
blue = gr.material({0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, 0, 0, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 0, 0, 0)

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
top:set_material(grass)
top:translate(-0.5, 0.5, -0.5)

right = gr.cube('right')
scene:add_child(right)
right:set_material(blue)
right:translate(0.5, -0.5, -0.5)

left = gr.cube('left')
scene:add_child(left)
left:set_material(red)
left:translate(-1.5, -0.5, -0.5)

-- s1 = gr.nh_sphere('s1', {0, -0.2, 0}, 0.15)
-- scene:add_child(s1)
-- s1:set_material(mat3)

white_light = gr.light({0, 0.47, 0.5}, {0.6, 0.6, 0.6}, {1, 0, 0}, 100000)

gr.render(scene, 'test.png', 256, 256,
	  {0, 0, 1.5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
