class Line{
    constructor(bodyA, pointB){
        var options = {
            bodyA: bodyA,
            pointB: pointB,
            stiffness: 0.005,
            length: 18
        }
        this.pointB = pointB;
        this.line = Constraint.create(options);
        World.add(world, this.line);
    }

    fly(){
        this.line.bodyA = null;
    }

    display(){
        if(this.line.bodyA){
            var pointA = this.line.bodyA.position;
            var pointB = this.pointB;
            strokeWeight(4);
            line(pointA.x, pointA.y, pointB.x, pointB.y);
        }
    }
    
}