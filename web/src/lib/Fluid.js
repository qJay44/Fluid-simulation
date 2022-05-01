class Fluid {
    constructor(dt, diffusion, viscosity) {
        this.size = N;           
        this.dt = dt;                       // time step
        this.diff = diffusion;              // diffusion amount
        this.visc = viscosity;              // viscosity
        
        this.s = new Array(N * N).fill(0);          // previous density
        this.density = new Array(N * N).fill(0);
        
        // velocity
        this.Vx = new Array(N * N).fill(0);
        this.Vy = new Array(N * N).fill(0);
        
        // previous velocity
        this.Vx0 = new Array(N * N).fill(0);
        this.Vy0 = new Array(N * N).fill(0);
    }
    
    step() {
        const visc    = this.visc;
        const diff    = this.diff;
        const dt      = this.dt;
        const Vx      = this.Vx;
        const Vy      = this.Vy;
        const Vx0     = this.Vx0;
        const Vy0     = this.Vy0;
        const s       = this.s;
        const density = this.density;
        
        diffuse(1, Vx0, Vx, visc, dt);
        diffuse(2, Vy0, Vy, visc, dt);
        
        project(Vx0, Vy0, Vx, Vy);
        
        advect(1, Vx, Vx0, Vx0, Vy0, dt);
        advect(2, Vy, Vy0, Vx0, Vy0, dt);
        
        project(Vx, Vy, Vx0, Vy0);
        
        diffuse(0, s, density, diff, dt);
        advect(0, density, s, Vx, Vy, dt);
    }

    addDensity(x, y, amount) {
        const index = IX(x, y);
        this.density[index] += amount;
    }

    addVelocity(x, y, amountX, amountY) {
        const index = IX(x, y);
        this.Vx[index] += amountX;
        this.Vy[index] += amountY;
    }

    renderDensity() {
        for (let i = 0; i < N; i++) {
            for (let j = 0; j < N; j++) {
                const x = i * SCALE;
                const y = j * SCALE;
                const d = this.density[IX(i, j)];

                ctx.fillStyle = `rgb(${d}, ${d}, ${d})`;
                d != 0 ? console.log(x, y) : false;
                ctx.fillRect(x, y, SCALE, SCALE);
                ctx.fill();
            }
        }
    }
}
