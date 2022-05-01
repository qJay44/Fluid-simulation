const canvas = document.querySelector('canvas');
const ctx = canvas.getContext('2d');

const w = canvas.width = N * SCALE;
const h = canvas.height = N * SCALE;

let fluid = new Fluid(0.1, 0, 0);

function draw() {

    fluid.step();
    fluid.renderDensity();
    
    requestAnimationFrame(draw);
}

canvas.onmousemove = (event) => {
    fluid.addDensity(event.x / SCALE, event.y / SCALE, 50);
}

draw();
