$fn = 50;
eps = 0.01;

p10_width = 320;
p10_height = 160;
p10_depth = 35;

border_size = 5;

hole_size = 9;
hole_gap = 1;
hole_z_gap = 5;

lid_height = 1.5;
lid_gap = 50;

top_screw_gap = 63;
side_screw_gap = 86;
screw_offset = 12;

hook_width = 3;
hook_height = 3;

//scale([1.05, 1.05, 1.05]) {
    difference() {
        union() {
            box();
            translate([0,0,p10_depth]) {
                resize([0,0,border_size],[false,false,true]) {
                    grid();
                }
            }
            translate([0,0,p10_depth+border_size]) hooks();
            *translate([0,0,lid_gap]) {
                lid();
            }
            *translate([0,0,p10_depth+lid_gap]) {
                resize([0,0,3],[false,false,true]) {
                    grid();
                }
            }
        }
        translate([0,0,p10_depth+lid_gap]) hooks_negative();
        //translate([0,0,+eps]) scale([1,1,1+eps*2]) screws();
        //translate([-border_size,-10,-10]) cube([p10_width/2+border_size,300,200]);
        //translate([p10_width/2,-10,-10]) cube([p10_width/2+border_size,300,200]);
    }
//}

module box() {
    difference() {
        minkowski() {
            cube([p10_width,p10_height,p10_depth+hole_z_gap]);
            difference() { 
                sphere(border_size);
                translate([-border_size,-border_size,0]) cube(border_size*2);
            }
        }
        cube([p10_width,p10_height,p10_depth+border_size+eps]);
    }
}

module grid() {
    union() {
        t = hole_size+hole_gap;
        z = hole_z_gap+lid_gap+lid_height+border_size;
        for (i = [1: 31]) {
            translate([i*t,-eps,0]) cube([hole_gap,p10_height+eps*2,z]);
        }
        for (i = [1: 15]) {
            translate([-eps,i*t,0]) cube([p10_width+eps*2,hole_gap,z]);
        }
    }
}
        
module screws() {
    translate([p10_width/2,p10_height/2,-border_size-10]) {
        x = top_screw_gap/2;
        y = -p10_height/2 + screw_offset;
        translate([x,y,0]) cylinder(h=p10_depth, r=4);
        translate([-x,y,0]) cylinder(h=p10_depth, r=4);
        
        translate([x,-y,0]) cylinder(h=p10_depth, r=4);
        translate([-x,-y,0]) cylinder(h=p10_depth, r=4);
        
        x2 = -p10_width/2+screw_offset;
        y2 = side_screw_gap/2;
        translate([x2,-y2,0]) cylinder(h=p10_depth, r=4);
        translate([x2,y2,0]) cylinder(h=p10_depth, r=4);
       
        translate([-x2,-y2,0]) cylinder(h=p10_depth, r=4);
        translate([-x2,y2,0]) cylinder(h=p10_depth, r=4);
        
        cylinder(h=p10_depth, r=15);
    }
}

module lid() {
    difference() {
        minkowski() {
            translate([0,0,p10_depth]) cube([p10_width,p10_height,lid_height]);
            difference() { 
                sphere(border_size);
                translate([-border_size,-border_size,-border_size*2]) cube(border_size*2);
            }
        }
        translate([0,0,p10_depth-eps]) cube([p10_width,p10_height,lid_height+border_size+eps*2]);
    }
}

module hook() {
    translate([-hook_width/2,0,0]) {
        hull() {
            cube([hook_width,hook_width*2,eps]);
            translate([0,0,hook_height]) cube([hook_width,hook_width*3,eps]);
        }
    }
}

module hook_negative() {
    hook();
    translate([-hook_width/2,-5,0]) cube([hook_width,5,hook_height]);
}

module hooks() {
    y = p10_height/2;
    g = 50;
    translate([-border_size/2,y-g,0]) hook();
    translate([-border_size/2,y+g,0]) hook();
    translate([p10_width+border_size/2,y-g,0]) hook();
    translate([p10_width+border_size/2,y+g,0]) hook();
}

module hooks_negative() {
    y = p10_height/2;
    g = 50;
    translate([-border_size/2,y-g,0]) scale([1.1,1.1,1.1]) {
        hook_negative();
    }
    translate([-border_size/2,y+g,0]) scale([1.1,1.1,1.1]) {
        hook_negative();
    }
    translate([p10_width+border_size/2,y-g,0]) scale([1.1,1.1,1.1]) {
        hook_negative();
    }
    translate([p10_width+border_size/2,y+g,0]) scale([1.1,1.1,1.1]) {
        hook_negative();
    }
}
   