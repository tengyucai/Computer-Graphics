brick = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 0, 'Assets/brick.png', 'Assets/brick_norm.png')
dirt = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 0, 'Assets/dirt.png', 'Assets/dirt_norm.png')
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 1.5)
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

white_light = gr.light({0, 0.5, 0.0}, {0.75, 0.75, 0.75}, {1, 0, 0}, 3, 10000000)

gr.render(scene, 'perlin.png', 512, 512,
	  {0, 0, 1.5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

