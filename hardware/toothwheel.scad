// dxf tooth data from http://oem.cadregister.com/asp/PPOW_Entry.asp?company=915217&elementID=07807803/METRIC/URETH/WV0025/F
// pulley diameter http://www.sdp-si.com/D265/HTML/D265T016.html

// tuneable constants

teeth = 227;			// Number of teeth, standard Mendel T5 belt = 8, gives Outside Diameter of 11.88mm

motor_shaft = 5.2;	// NEMA17 motor shaft exact diameter = 5
m3_dia = 3.2;		// 3mm hole diameter
m3_nut_hex = 1;		// 1 for hex, 0 for square nut
m3_nut_flats = 5.7;	// normal M3 hex nut exact width = 5.5
m3_nut_depth = 2.7;	// normal M3 hex nut exact depth = 2.4, nyloc = 4

pulley_t_ht = 8;	// length of toothed part of pulley, standard = 12
pulley_b_ht = 8;		// pulley base height, standard = 8. Set to same as idler_ht if you want an idler but no pulley.
pulley_b_dia = 20;	// pulley base diameter, standard = 20
no_of_nuts = 1;		// number of captive nuts required, standard = 1
nut_angle = 90;		// angle between nuts, standard = 90
nut_shaft_distance = 1.2;	// distance between inner face of nut and shaft, can be negative.

//	** Scaling tooth for good fit **
//  We are modelling the *BELT* tooth here, not the tooth on the pulley. Increasing the number will *decrease* the pulley tooth size. Increasing the tooth width will also scale proportionately the tooth depth, to maintain the shape of the tooth, and increase how far into the pulley the tooth is indented. Can be negative

additional_tooth_width = 0.2; //mm

//	this will cause the shape of the tooth to change.
additional_tooth_depth = 0; //mm

// set the diameter for a given number of teeth
HTD_5mm_pulley_dia = tooth_spacing (5,0.5715);

// creation part, passes the diameter and tooth width
projection (cut = true) 
pulley ( "HTD 5mm" , HTD_5mm_pulley_dia , 2.199 , 3.781 ); 


// Functions

function tooth_spaceing_curvefit (b,c,d)
	= ((c * pow(teeth,d)) / (b + pow(teeth,d))) * teeth ;

function tooth_spacing(tooth_pitch,pitch_line_offset)
	= (2*((teeth*tooth_pitch)/(3.14159265*2)-pitch_line_offset)) ;

// Main Module

module pulley( belt_type , pulley_OD , tooth_depth , tooth_width )
	{
	echo (str("Belt type = ",belt_type,"; Number of teeth = ",teeth,"; Pulley Outside Diameter = ",pulley_OD,"mm "));
	tooth_distance_from_centre = sqrt( pow(pulley_OD/2,2) - pow((tooth_width+additional_tooth_width)/2,2));
	tooth_width_scale = (tooth_width + additional_tooth_width ) / tooth_width;
	tooth_depth_scale = ((tooth_depth + additional_tooth_depth ) / tooth_depth) ;


	difference()
	{	 
		difference()
			{
			//shaft - diameter is outside diameter of pulley
			
			//translate([0,0,pulley_b_ht]) 
			rotate ([0,0,360/(teeth*4)]) 
			cylinder(r=pulley_OD/2,h=pulley_t_ht, $fn=teeth*4);
	
			//teeth - cut out of shaft
		
			for(i=[1:teeth]) 
			rotate([0,0,i*(360/teeth)])
			translate([0,-tooth_distance_from_centre,-1]) 
			scale ([ tooth_width_scale , tooth_depth_scale , 1 ]){HTD_5mm();}
            
            
            //hole for motor shaft
            translate([0,0,0])  cylinder(r=80 ,h=10); //,$fn=motor_shaft*4);
                
            //translate([157.5*cos(120), 107*sin(120),0]) cylinder(r=2.6 ,h=10);
			}	
            
         translate([-200,0,0]) cube([400,1,10],0) ;   
            
	}
}

// Tooth profile modules

module HTD_5mm()
	{
	linear_extrude(height=pulley_t_ht+2) polygon([[-1.89036,-0.75],[-1.89036,0],[-1.741168,0.02669],[-1.61387,0.100806],[-1.518984,0.21342],[-1.467026,0.3556],[-1.427162,0.960967],[-1.398568,1.089602],[-1.359437,1.213531],[-1.310296,1.332296],[-1.251672,1.445441],[-1.184092,1.552509],[-1.108081,1.653042],[-1.024167,1.746585],[-0.932877,1.832681],[-0.834736,1.910872],[-0.730271,1.980701],[-0.62001,2.041713],[-0.504478,2.09345],[-0.384202,2.135455],[-0.259708,2.167271],[-0.131524,2.188443],[-0.000176,2.198511],[0.131296,2.188504],[0.259588,2.167387],[0.384174,2.135616],[0.504527,2.093648],[0.620123,2.04194],[0.730433,1.980949],[0.834934,1.911132],[0.933097,1.832945],[1.024398,1.746846],[1.108311,1.653291],[1.184308,1.552736],[1.251865,1.445639],[1.310455,1.332457],[1.359552,1.213647],[1.39863,1.089664],[1.427162,0.960967],[1.467026,0.3556],[1.518984,0.21342],[1.61387,0.100806],[1.741168,0.02669],[1.89036,0],[1.89036,-0.75]]);
	}

