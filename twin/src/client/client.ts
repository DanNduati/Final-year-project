import * as THREE from 'three'
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls'
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader'
import Stats from 'three/examples/jsm/libs/stats.module'

const scene = new THREE.Scene()
scene.add(new THREE.AxesHelper(5))

/*
const light = new THREE.DirectionalLight(0xffffff, 1);
light.position.set(2, 2, 5);
scene.add(light);
*/
let bucketRoot;
const light = new THREE.HemisphereLight(0xffffbb, 0x080820, 1);
scene.add(light);
const camera = new THREE.PerspectiveCamera(
    75,
    window.innerWidth / window.innerHeight,
    0.1,
    100000
)
camera.position.z = 5

const renderer = new THREE.WebGLRenderer()
renderer.physicallyCorrectLights = true
renderer.shadowMap.enabled = true
renderer.outputEncoding = THREE.sRGBEncoding
renderer.setSize(window.innerWidth, window.innerHeight)
document.body.appendChild(renderer.domElement)

const controls = new OrbitControls(camera, renderer.domElement)
controls.enableDamping = true;

const bucketLoader = new GLTFLoader()
bucketLoader.load(
    'models/scene(1).glb',
    function (gltf) {
        bucketRoot = gltf.scene;
        bucketRoot.scale.set(0.0042, 0.0042, 0.0042);
        bucketRoot.position.set(0.4, 0.25, 0.42);
        scene.add(bucketRoot)

    },
    (xhr) => {
        console.log((xhr.loaded / xhr.total) * 100 + '% loaded')
    },
    (error) => {
        console.log(error)
    }
)

const frameLoader = new GLTFLoader()
frameLoader.load(
    'models/scene.glb',
    function (gltf) {
        const frameRoot = gltf.scene;
        frameRoot.scale.set(0.003, 0.003, 0.003);
        scene.add(frameRoot)
    },
    (xhr) => {
        console.log((xhr.loaded / xhr.total) * 100 + '% loaded')
    },
    (error) => {
        console.log(error)
    }
)


window.addEventListener('resize', onWindowResize, false)
function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight
    camera.updateProjectionMatrix()
    renderer.setSize(window.innerWidth, window.innerHeight)
    render()
}

const stats = Stats()
document.body.appendChild(stats.dom)

function animate() {
    requestAnimationFrame(animate);
    //controls.update()
    let y_pos = bucketRoot.position.y;
    console.log(y_pos);
    bucketRoot.position.y += 0.001;
    render();
    //stats.update()
}

function render() {
    renderer.render(scene, camera)
}

animate()