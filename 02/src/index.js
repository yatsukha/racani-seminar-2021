import * as THREE from 'three';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(
  60, // field of view
  window.innerWidth / window.innerHeight, // aspect ratio
  0.1, // near clipping
  1000 // far clipping
);

camera.position.set(30, 30, 30);
camera.lookAt(0, 0, 0);

const renderer = new THREE.WebGLRenderer({ antialias: true, });
renderer.setSize(window.innerWidth, window.innerHeight);

document.body.appendChild(renderer.domElement);

let objects = [];

function rotateAboutPoint(obj, point, axis, theta, pointIsWorld){
    pointIsWorld = (pointIsWorld === undefined)? false : pointIsWorld;

    if(pointIsWorld){
        obj.parent.localToWorld(obj.position); // compensate for world coordinate
    }

    obj.position.sub(point); // remove the offset
    obj.position.applyAxisAngle(axis, theta); // rotate the POSITION
    obj.position.add(point); // re-add the offset

    if(pointIsWorld){
        obj.parent.worldToLocal(obj.position); // undo world coordinates compensation
    }

    obj.rotateOnAxis(axis, theta); // rotate the OBJECT
}

function generateSphere() {

  const geometry = new THREE.SphereGeometry(1 + Math.random() - 0.5, 16, 8);
  const material = new THREE.MeshBasicMaterial({ color:  0x000000 });
  const sphere = new THREE.Mesh(geometry, material);

  //sphere.matrixAutoUpdate = false;

  sphere.position.add(new THREE.Vector3(
    20 * (Math.random() - 0.5), 
    20 * (Math.random() - 0.5), 
    20 * (Math.random() - 0.5)
  ));

  //sphere.matrixAutoUpdate = false;

  const d_r = Math.random() * 0.003; 
  const d_g = Math.random() * 0.003;
  const d_b = Math.random() * 0.003;

  const max_life = 1.5 / Math.max(Math.max(d_r, d_g), d_b);

  const d_x = Math.random() * 10;
  const d_y = Math.random() * 10;
  const d_z = Math.random() * 10;

  const rotation_speed = Math.random() * 0.01;

  return {
    mesh: sphere,
    current_life: 0,
    max_life: max_life,
    update: function() {
      this.current_life += 1;
      if (this.current_life >= this.max_life) {
        return true;
      }

      //let color = this.mesh.material.color.getHex();
      //color += d_r << 16;
      //color += d_g << 8;
      //color += d_b;

      this.mesh.material.color.r += d_r;
      this.mesh.material.color.g += d_g;
      this.mesh.material.color.b += d_b;
      //this.mesh.material.color.setHex(color);
      rotateAboutPoint(
        this.mesh, 
        new THREE.Vector3(0, 0, 0), 
        (new THREE.Vector3(d_x, d_y, d_z)).normalize(), 
        rotation_speed, 
        true
      );

      return false;
    }
  };
}

for (let i = 0; i < 250; ++i) {
  objects.push(generateSphere());
  scene.add(objects[i].mesh);
}

let running = false;

document.addEventListener('keydown', function (e) {
  if (e.code == "Space") {
    running = !running;
  }
});

async function animate() {
  if (!running) {
    await (function() {
      return new Promise((resolve) => {
        document.addEventListener('keydown', function (e) {
          if (e.code == "Space") {
            resolve();
          }
        });
      });
    })();
    running = true;
  }

  requestAnimationFrame(animate);

  for (let i = 0; i < objects.length; ++i) {
    if (objects[i].update()) {
      scene.remove(objects[i].mesh);
      objects[i] = generateSphere();
      scene.add(objects[i].mesh);
    }
  }

  renderer.render(scene, camera);
}

animate();
