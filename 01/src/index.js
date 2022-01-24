import * as THREE from 'three';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(
  100, // field of view
  window.innerWidth / window.innerHeight, // aspect ratio
  0.1, // near clipping
  1000 // far clipping
);

camera.position.set(5, 5, 0.5);
camera.lookAt(5, 5, 0);

const renderer = new THREE.WebGLRenderer({ antialias: true, });
renderer.setSize(window.innerWidth, window.innerHeight);

document.body.appendChild(renderer.domElement);

function splineFromControlPoints(points, segments = 200) {

  let spline = [];
  let tangent = [];
  const delta = 1 / segments;
  
  for (let i = 1; i < points.length - 2; ++i) {
    const base = new THREE.Matrix4();
    base.set(
      -1,  3, -3, 1,
       3, -6,  3, 0,
      -3,  0,  3, 0,
       1,  4,  1, 0
    );
    base.multiplyScalar(1 / 6);

    const pointMatrix = new THREE.Matrix4();

    const v0 = points[i - 1];
    const v1 = points[i];
    const v2 = points[i + 1];
    const v3 = points[i + 2];

    pointMatrix.set(
      v0.x, v0.y, v0.z, 0, 
      v1.x, v1.y, v1.z, 0, 
      v2.x, v2.y, v2.z, 0, 
      v3.x, v3.y, v3.z, 0
    );

    const e = base.multiply(pointMatrix).elements;

    for (let t = 0.0; t <= 1.0; t += delta) {
      let t2 = t * t;
      let t3 = t * t2;

      spline.push(new THREE.Vector3(
        e[0] * t3 + e[1] * t2 + e[2] * t + e[3],
        e[4] * t3 + e[5] * t2 + e[6] * t + e[7],
        e[8] * t3 + e[9] * t2 + e[10] * t + e[11]
      ));

      tangent.push(new THREE.Vector3(
        3 * e[0] * t2 + 2 * e[1] * t + e[2],
        3 * e[4] * t2 + 2 * e[5] * t + e[6],
        3 * e[8] * t2 + 2 * e[9] * t + e[10]
      ));
    }
  }

  return {
    spline: spline,
    tangent: tangent,
  };
}

const controlPoints = [];

for (let i = 0; i < 40; ++i) {
  controlPoints.push(new THREE.Vector3(
    //Math.random() * 10,
    //Math.random() * 10,
    (Math.floor(i / 2) % 2) * 10,
    (Math.floor((i + 1) / 2) % 2) * 10,
    i * 3
  ));
}

scene.add(new THREE.Line(
  new THREE.BufferGeometry().setFromPoints(controlPoints),
  new THREE.LineDashedMaterial({ color: 0xffffff, dashSize: 1, gapSize: 0.2 })
).computeLineDistances());

const bSpline = splineFromControlPoints(controlPoints);
const points = bSpline.spline;
const tangents = bSpline.tangent;

scene.add(new THREE.Line(
  new THREE.BufferGeometry().setFromPoints(points),
  new THREE.LineBasicMaterial({ color: 0xffffff })
));

const loader = new OBJLoader();
let object;

loader.load(
  'teddy.obj',
  obj => {
    obj.traverse(function (child) {
      if (child instanceof THREE.Mesh) {
        child.material = new THREE.MeshBasicMaterial({ color: 0xff00ff, wireframe: false });
        child.geometry.center();
        child.geometry.scale(0.2, 0.2, 0.2);
        child.rotateZ(3.14);
        child.matrixAutoUpdate = false;
        scene.add(object = child);
      }
    });
  },
  _ => {},
  console.log
);

let running = false;
let index = 0;
let index_delta = 2;

document.addEventListener('keydown', function (e) {
  if (e.code == "Space") {
    running = !running;
  } else if (e.code == "KeyT") {
    const tangent = tangents[index].clone().normalize().multiplyScalar(5);
    scene.add(new THREE.Line(
      new THREE.BufferGeometry().setFromPoints([
        points[index].clone().sub(tangent.clone().multiplyScalar(1.5)),
        points[index].clone().add(tangent)
      ]),
      new THREE.LineBasicMaterial({ color: 0xffffff })
    ));
  } else if (e.key == "+") {
    index_delta += 1;
  } else if (e.key == "-") {
    index_delta -= 1;
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

  if (index >= 0 && index < points.length) {
    const objectOrientation = new THREE.Vector3(0, 0, 1);
    objectOrientation.applyQuaternion(object.quaternion);
    const wantedOrientation = tangents[index];

    if (index_delta < 0) {
      wantedOrientation.multiplyScalar(-1);
    }

    const rotationAxis = objectOrientation.clone().cross(wantedOrientation).normalize();
    const rotationAngle = Math.acos( 
      objectOrientation.dot(wantedOrientation)
        / (
          Math.sqrt(objectOrientation.dot(objectOrientation))
          * Math.sqrt(wantedOrientation.dot(wantedOrientation))
        ));

    object.matrix.makeRotationAxis(rotationAxis, rotationAngle); 
    object.matrix.setPosition(points[index]);

    camera.position.z = 10 + points[index].z;
    index += index_delta;
  } else {
    running = false;
  }
  renderer.render(scene, camera);
}

animate();
