// Generic Pallet Module

include <materials.scad>

module Pallet(length, width, center) {
	// Define parts
	Rails = [Rails_W, length, Rails_T];
	Frame = [Frame_W, width, Frame_T];
	Tie = [Tie_W, width, Frame_T];

	// Place Foundation
	//  The end-to-end constant 29.25 is used because in the pallets I have made I can find no edge-cases that would need to alter it and I lack sufficient data on pallet jacks to automaticly place them.
	foundation_pos = [0, 29.25 - Frame_W, length - 29.25, length - Frame_W];

	for( pos = foundation_pos ) {
		rotate([0,0,90]) translate([pos - length / 2, - width / 2, - (Rails_W / 2) - Frame_T]) cube(Frame);
	}

	for( pos = [0, length - Frame_W] ) {
		rotate([0,0,90]) translate([pos - length / 2, - width / 2, Rails_W /2]) cube(Frame);
	}

	// Place Rails
	//  The end-to-end constant for the space between the two middle rails is 13.5
	//  Again I lack sufficient data on pallet jacks to automaticly place them.
	
	rail_spaces = (width - 13.5 + Rails_T) / 2;

	rail_pos = [0, rail_spaces, width - rail_spaces, width - Rails_T];

	for( pos = rail_pos ) {
		translate([pos - width /2 , -length /2 ,Rails_W /2]) rotate([0,90,0]) cube(Rails);
	}

	// Place Ties
	//  Ties form the main face of the pallet
   //  In this design, a centeral cluster of ties is used to reinforce the middle of the pallet
	pallet_center = length / 2;
	cluster_center = (center * Tie_W) / 2 + Tie_W;
	function cluster_pos_gen(ittr) = pallet_center - cluster_center + (ittr * Tie_W);
	cluster_pos = [ for(i=[1:center]) cluster_pos_gen(i) ];
    for( pos = cluster_pos ){
        rotate([0,0,90]) translate([pos - length / 2, - width / 2, Rails_W /2]) cube(Tie);
    }
    
    target_space = 8;
    tie_count = ceil((length - ( center * Tie_W + 2 * ( Frame_W + target_space ))) / (Tie_W + target_space ) / 2) * 2;
    tie_space = (length - ( center * Tie_W + 2 * Frame_W + tie_count * Tie_W )) / (tie_count + 2);
    echo(tie_count);
    echo(tie_space);
    for(i = [0:tie_count / 2 - 1] ) {
        rotate([0,0,90]) translate([i * (tie_space + Tie_W) + (center * Tie_W) / 2 + tie_space, - width / 2, Rails_W /2]) cube(Tie);
    }
     for(i = [0:tie_count / 2 - 1] ) {
        rotate([0,0,90]) translate([-(i * (tie_space + Tie_W) + (center * Tie_W) / 2 + tie_space) - Tie_W, - width / 2, Rails_W /2]) cube(Tie);
    }
}