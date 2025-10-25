return {
    init = function(self, entity, world)
    end,

    update = function(self, entity, world, deltaTime, inputHandler)
        local camera = world:get_camera(entity)
        if not camera then return end

        local speed = 10.0
        local movement = Vec3.new(0.0, 0.0, 0.0)

        local forward = camera:front()
        local up = camera:up()
        local right = forward:cross(up):normalize()

        if inputHandler:isKeyPressed(Keys.W) then
            movement = movement + forward
        end
        if inputHandler:isKeyPressed(Keys.S) then
            movement = movement - forward
        end
        if inputHandler:isKeyPressed(Keys.A) then
            movement = movement - right
        end
        if inputHandler:isKeyPressed(Keys.D) then
            movement = movement + right
        end
        if inputHandler:isKeyPressed(Keys.E) then
            movement = movement + up
        end
        if inputHandler:isKeyPressed(Keys.Q) then
            movement = movement - up
        end

        if movement:length() > 0.0 then
            movement = movement:normalize() * speed * deltaTime
            camera:setPosition(camera:position() + movement)
        end
    end
}
