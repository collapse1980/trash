const Engine = Matter.Engine;
const World = Matter.World;
const Bodies = Matter.Bodies;
const Body = Matter.Body;
const Constraint = Matter.Constraint;

var paper, paperimg;
var dustbin2, dustbin1, dustbin3, logimg;
var world;
var line1;

function setup() {
	createCanvas(1200, 400);
  rectMode(CENTER);

	engine = Engine.create();
	world = engine.world;

  paper = new Paper(170, 32, 80); 
  paper.scale = 0.1;

	//Create a Ground
  ground = new Ground(600, height, 1200, 20);
  dustbin1 = new dustbin (800, 368);
  line1 = new Line(paper.body, {x:200, y:100});
  //dustbin2 = new Dustbin (850, 320, 150, 150);
  //dustbin3 = new Dustbin (800, 300, 150, 150);	

  Engine.run(engine);
  
}


function draw() {
  rectMode(CENTER);
  background(255);
  
  
  //drawSprites();

  
  paper.display();
  ground.display();
  dustbin1.display();
  line1.display();
  //dustbin2.display();
  
  //dustbin3.display();

  //drawSprites();
 
}

//function keyPressed(){
  //if (keyCode === UP_ARROW) {
    //  Matter.Body.applyForce(paper.body, paper.body.position, {x:1180, y:-1180});
  //}
//}

function mouseDragged(){
  Matter.Body.setPosition(paper.body, {x:mouseX, y:mouseY});
}

function mouseReleased(){
  line1.fly();
}
  

