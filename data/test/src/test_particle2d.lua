local test = require("test")
local random = require("random")
local particle = require("particle")
local rnd = random.pcg32_fast()
rnd:seed(114514)

local particle_pool;

---@class test.Module.Texture : test.Base
local M = {}

function M:onCreate()
    local old_pool = lstg.GetResourceStatus()
    lstg.SetResourceStatus("global")
    lstg.LoadTexture('tex:white', 'res/white.png')
    lstg.LoadImage('img:white', 'tex:white', 0, 0, lstg.GetTextureSize('tex:white'))
    lstg.SetResourceStatus(old_pool)


    particle_pool = particle.NewPool2D("img:white", "mul+add", 8192 / 32)
end

function M:onDestroy()

end

function M:onUpdate()
    local scale = 1.6
    for _ = 1, 128 / 32 do
        local pdx = rnd:number(-2, 2) * scale
        local p = particle_pool:AddParticle(500, 480, 90, pdx, rnd:number(-0.1, 1) * scale, 0.8 * scale)
        p.ax = -pdx / 40
        p.ay = rnd:number(0.06)
        -- p.omiga = rnd:number(-1, 1)
        p.color = lstg.Color(0xFFCC2200)
    end
    particle_pool:Update()
end

function M:onRender()
    window:applyCameraV()
    lstg.RenderClear(lstg.Color(0xFF000000))

    particle_pool:Render()
end

test.registerTest("test.Module.Particle2D", M)
