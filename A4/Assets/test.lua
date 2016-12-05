grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 0, 0, 2.0)

scene = gr.node('root')

-- the floor
plane = gr.mesh( 'plane', 'Assets/untitled.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:rotate('X', 60)
plane:scale(1,1,1)


-- The lights
l1 = gr.light({0,0,200}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'test.png', 256, 256, 
	  {0, 0, 200}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})