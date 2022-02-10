const firebaseConfig = {
    apiKey: "AIzaSyBpX0r75at6hZEfyjTC6iydB63elHBSCWc",
    authDomain: "elevator-3a561.firebaseapp.com",
    databaseURL: "https://elevator-3a561-default-rtdb.firebaseio.com",
    projectId: "elevator-3a561",
    storageBucket: "elevator-3a561.appspot.com",
    messagingSenderId: "469815765021",
    appId: "1:469815765021:web:f18d43deb2c8ac1b2cfef4"
};
// Initialize Firebase app
firebase.initializeApp(firebaseConfig);

//elevator queue logic
let currentFloor = null;
let distance = 0.0;
let floors = [0, 1, 2];
let ismoving = false;
let destinyFloors = [];
let elevatorStatus = 'idle';
let leavingFloor = false;


const distanceVal = firebase.database().ref('elevatorData/distance/value');
distanceVal.on('value', (snapshot) => {
    distance = snapshot.val()
    //console.log(distance);
});

const scene = new THREE.Scene();
scene.add(new THREE.AxesHelper(5));
scene.background = new THREE.Color(0xffffff);

const light = new THREE.PointLight(0xffffff, 10, 100);
light.position.set(-50, 1, -50);
scene.add(light);

const light2 = new THREE.HemisphereLight(0xffffff, 10);
light2.position.set(0, 100, 0);
scene.add(light2);

const light3 = new THREE.HemisphereLight(0xffffff, 10);
light2.position.set(-20, 0, 0);
scene.add(light3);

const light4 = new THREE.HemisphereLight(0xffffff, 10);
light2.position.set(0, 0, 20);
scene.add(light4);

const directionalLight = new THREE.DirectionalLight(0xffffff, 4);
scene.add(directionalLight);


const spotLight = new THREE.SpotLight(0xffffff, 0.5);
spotLight.position.set(5, 100, 5);
scene.add(spotLight);

const light_ = new THREE.HemisphereLight(0xffffbb, 0x080820, 3);
scene.add(light_);
const light_2 = new THREE.DirectionalLight(0xffffff, 4);
light_2.position.set(2, 20, 5);
scene.add(light_2);
const light_3 = new THREE.AmbientLight(0x404040); // soft white light
scene.add(light_3);
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
camera.position.x = -2.941;
camera.position.y = 5.72;
camera.position.z = 0.7994;

camera.lookAt(scene.position);
scene.add(camera);


const renderer = new THREE.WebGLRenderer();
renderer.physicallyCorrectLights = true;
renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);

const controls = new THREE.OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;

const elevatorLoader = new THREE.GLTFLoader();
//objects
let bucket, frame, sprockets, counterWeight;
elevatorLoader.load('models/bucket.glb', function (gltf) {
    bucket = gltf.scene;
    bucket.scale.set(3.5, 3.5, 3.5);
    scene.add(bucket);

}, function (xhr) {
    console.log((xhr.loaded / xhr.total) * 100 + '% loaded');

}, function (error) {

    console.log(error);

});
elevatorLoader.load('models/Elevator.glb', function (gltf) {
    frame = gltf.scene;
    frame.scale.set(3.5, 3.5, 3.5);
    scene.add(frame);

}, function (xhr) {
    console.log((xhr.loaded / xhr.total) * 100 + '% loaded');

}, function (error) {

    console.log(error);

});
elevatorLoader.load('models/sprockets.glb', function (gltf) {
    sprockets = gltf.scene;
    sprockets.scale.set(3.5, 3.5, 3.5);
    scene.add(sprockets);

}, function (xhr) {
    console.log((xhr.loaded / xhr.total) * 100 + '% loaded');

}, function (error) {

    console.log(error);

});
elevatorLoader.load('models/counterweight.glb', function (gltf) {
    counterWeight = gltf.scene;
    counterWeight.scale.set(3.5, 3.5, 3.5);
    scene.add(counterWeight);

}, function (xhr) {
    console.log((xhr.loaded / xhr.total) * 100 + '% loaded');

}, function (error) {

    console.log(error);

});

window.addEventListener('resize', onWindowResize, false);
function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
    render();
}
function animate() {
    requestAnimationFrame(animate);
    //console.log(`x: ${camera.position.x}, y: ${camera.position.y}, z: ${camera.position.z}`);
    console.log(bucket.position.y);
    bucket.position.y += 0.001;
    //bucket.position.y = distance / 500;
    //counterWeight.position.y -= 0.005;
    //sprockets.rotation.z += 0.05;
    controls.update();
    render();
}
function render() {
    renderer.render(scene, camera);
}
animate();
