return {
    m_elapsed = 0,

    init = function(self, entity, world)
        local t = world:get_transform(entity)
        if not t then return end
        self.startY = t.position.y
    end,

    update = function(self, entity, world, dt)
        local t = world:get_transform(entity)
        if not t then return end
        self.m_elapsed = self.m_elapsed + dt
        local offset = math.sin(self.m_elapsed * 2 * math.pi)
        t.position.y = self.startY + offset
    end
}
