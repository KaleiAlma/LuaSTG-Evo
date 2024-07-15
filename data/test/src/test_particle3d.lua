local test = require("test")
local particle = require("particle")
local random = require("random")
local rnd = random.pcg32_fast()
rnd:seed(114514)

local particle_pool;

local function Camera3D()
    ---@class kuanlan.Camera3D
    local M = {
        dir = 90,
        upd = 0,
        x = 0,
        y = 2,
        z = -10,
        speed = 0.05,

        rbtn = false,
        mx = 0,
        my = 0,
        last_dir = 90,
        last_upd = 0,
        aspeed = 0.1,

        fog_near = 5,
        for_far = 20,
    }
    function M:update()
        local speed = self.speed
        local Key = lstg.Input.Keyboard
        if lstg.GetKeyState(Key.LeftControl) then
            speed = speed * 10.0
        end
        if lstg.GetKeyState(Key.W) then
            self.x = self.x + speed * math.cos(math.rad(self.dir))
            self.z = self.z + speed * math.sin(math.rad(self.dir))
        elseif lstg.GetKeyState(Key.S) then
            self.x = self.x - speed * math.cos(math.rad(self.dir))
            self.z = self.z - speed * math.sin(math.rad(self.dir))
        end
        if lstg.GetKeyState(Key.D) then
            self.x = self.x + speed * math.cos(math.rad(self.dir - 90))
            self.z = self.z + speed * math.sin(math.rad(self.dir - 90))
        elseif lstg.GetKeyState(Key.A) then
            self.x = self.x + speed * math.cos(math.rad(self.dir + 90))
            self.z = self.z + speed * math.sin(math.rad(self.dir + 90))
        end
        if lstg.GetKeyState(Key.Space) then
            self.y = self.y + speed
        elseif lstg.GetKeyState(Key.LeftShift) then
            self.y = self.y - speed
        end
        if not self.rbtn and lstg.GetMouseState(2) then
            self.rbtn = true
            self.mx, self.my = lstg.GetMousePosition()
            self.last_dir = self.dir
            self.last_upd = self.upd
        elseif self.rbtn and not lstg.GetMouseState(2) then
            self.rbtn = false
        end
        if self.rbtn then
            local mx, my = lstg.GetMousePosition()
            local dx, dy = mx - self.mx, my - self.my
            self.dir = self.last_dir - self.aspeed * dx
            self.upd = self.last_upd + self.aspeed * dy
            self.upd = math.max(-89.0, math.min(self.upd, 89.0))
        end
    end
    function M:apply()
        local tx, ty, tz = 1, 0, 0
        local ux, uy, uz = 0, 1, 0
        local function vec2_rot(x, y, r_deg)
            local sin_v = math.sin(math.rad(r_deg))
            local cos_v = math.cos(math.rad(r_deg))
            return x * cos_v - y * sin_v, x * sin_v + y * cos_v
        end
        tx, ty = vec2_rot(tx, ty, self.upd)
        ux, uy = vec2_rot(ux, uy, self.upd)
        tx, tz = vec2_rot(tx, tz, self.dir)
        ux, uz = vec2_rot(ux, uz, self.dir)
        lstg.SetViewport(0, window.width, 0, window.height)
        lstg.SetScissorRect(0, window.width, 0, window.height)
        lstg.SetPerspective(
            self.x, self.y, self.z,
            self.x + tx, self.y + ty, self.z + tz,
            ux, uy, uz,
            math.rad(80), window.width / window.height,
            0.01, 1000.0
        )
        lstg.SetImageScale(1)
        lstg.SetFog()
        --lstg.SetFog(self.fog_near, self.for_far, lstg.Color(255, 255, 255, 255))
    end
    return M
end

local camera3d = Camera3D()

---@class test.Module.Model : test.Base
local M = {}

function M:onCreate()
    local old_pool = lstg.GetResourceStatus()
    lstg.SetResourceStatus("global")
    lstg.LoadTexture('tex:white', 'res/white.png')
    lstg.LoadImage('img:white', 'tex:white', 0, 0, lstg.GetTextureSize('tex:white'))
    lstg.SetResourceStatus(old_pool)


    particle_pool = particle.NewPool3D("img:white", "mul+add", 8192 / 4)
end

function M:onDestroy()
    lstg.RemoveResource("global", 1, "tex:white")
    lstg.RemoveResource("global", 2, "img:white")
    
end

-- local timer = 0

function M:onUpdate()
    camera3d:update()
    -- timer = timer + 1
    
    for _ = 1, 128 / 4 do
        -- local pdx, pdz = rnd:number(-0.1, 0.1), rnd:number(-0.1, 0.1)
        local pda, pds = rnd:number(360), rnd:number(0.08)
        local pdx, pdz = pds * lstg.cos(pda), pds * lstg.sin(pda)
        local p = particle_pool:AddParticle(0, 0.2, 0, pdx, rnd:number(-0.001, 0.1), pdz)
        p.ox = rnd:number()
        p.oy = rnd:number()
        p.oz = rnd:number()
        p.ax = -pdx / 40
        p.ay = rnd:number(0.0006)
        p.az = -pdz / 40
        p.sx = 0.016
        p.sy = 0.016
        p.color = lstg.Color(0xFFCC2200)
    end

    particle_pool:Update()
end

function M:onRender()
    camera3d:apply()
    lstg.RenderClear(lstg.Color(0xFF000000))
    -- local scale = 0.05
    -- lstg.SetZBufferEnable(1)
    lstg.ClearZBuffer(1.0)

    lstg.SetImageState('img:white', '', lstg.Color(0xFF333333))
    lstg.Render3D('img:white', 0, -0.2, 0, 90, 0, 0, 0.3)

    particle_pool:Render()
    lstg.SetZBufferEnable(0)
end

test.registerTest("test.Module.Particle3D", M)
