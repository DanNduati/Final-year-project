//To display anything with three.js we need : scene,camera and renderer
//so that we can render the scene with the camera
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
const renderer = new THREE.WebGLRenderer();
//set size at which we want the renderer to render our app
renderer.setSize(window.innerWidth, window.innerHeight);
//add the renderer to the html document
document.body.appendChild(renderer.domElement);
//Cube
//BoxGeometry -> object that contains all the points(vertices) and fill(faces) of the cube
const geometry = new THREE.BoxGeometry();
//MeshBasicMatrial -> material to color it,all materials take an object of properties which will be applied to them
const material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
// Create a Mesh -> this is an object that takes a geometry and applies a material to it
const cube = new THREE.Mesh(geometry, material);
//insert the object to the scene -> by default it is added to (0,0,0)
scene.add(cube);
//since the camera and the cube are inside each other lol (0,0,0) move the camera out a bit
camera.position.z = 5;
//render the scene -> 
function animate() {
    //create a loop that causes the renderer to to draw the scene every time the screen is refreshed
    requestAnimationFrame(animate); //requestAnimationFrame has a number of advantages. Perhaps the most important one is that it pauses when the user navigates to another browser tab, hence not wasting their precious processing power and battery life.
    //animating the cube -> this will run in every frame
    cube.rotation.x += 0.01;
    cube.rotation.y += 0.01;

    renderer.render(scene, camera);
};
// call render/animate loop
animate();