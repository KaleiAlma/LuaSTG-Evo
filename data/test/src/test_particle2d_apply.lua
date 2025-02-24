local test = require("test")
local particle = require("particle")
local random = require("random")
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

local choose = { 'r', 'g', 'b' }

local timer = 0
local x, y = 800, 400
function M:onUpdate()
    if lstg.Input.Mouse.GetKeyState(lstg.Input.Mouse.Left) then
        x, y = lstg.Input.Mouse.GetPosition()
    end
    local scale = 1.6
    local flicker = lstg.sin(timer * 15) * 0.4 + rnd:number(-1, 1)
    for _ = 1, 128 / 32 do
        local pdx = rnd:number(-1, 1) * scale
        local p = particle_pool:AddParticle(x + flicker * -8, y, 0, pdx * 2.1 + flicker * 0.8, rnd:number(-1, 1) * scale, 1 * scale)
        p.ax = -pdx * 0.09
        p.ay = rnd:number(0.06, 0.18) * scale
        p.omiga = rnd:number(-1, 1)
        -- p.color = lstg.Color(0xFF0A22CC)
        -- p.color = lstg.HSVColor(100, rnd:integer(100), 100, 88)
        -- p.color = lstg.Color(0xFF000000 + bit.lshift(0x77, rnd:integer(2) * 8))
        p.r = 0
        p.g = 0
        p.b = 0
        p[choose[rnd:integer(1, 3)]] = 255
    end
    particle_pool:Update()
    -- local amt = 0
    particle_pool:Apply(function(p)
        -- amt = amt + 1
        -- if p.timer > 25 and r == 0 then
        --     return true
        -- end
        p.a = p.a * (0.999999 * (1 - p.timer * 0.00000045))
        if p.timer % 4 ~= 0 then return end
        local r = rnd:integer(13)
        if r == 6 --[[or math.abs(p.x - x) > 84 * scale]] then
            p.vx = p.vx * 0.5
            if p.vx * p.ax > 0 then
                p.ax = -p.ax
            end
        end
        if r == 0 or (p.y - y > 40 * scale and p.ay > 0) then
            p.ay = -p.ay * 0.3
        end
        if p.y - y < -8 * scale then
            p.vy = p.vy * 0.3
            if p.ay < 0 then
                p.ay = -p.ay * 2.1
            end
        end
    end)
    timer = timer + 1
    -- print(amt)
end

function M:onRender()
    window:applyCameraV()
    lstg.RenderClear(lstg.Color(0xFF000000))

    particle_pool:Render()
end

test.registerTest("test.Module.Particle2D.Apply", M)
