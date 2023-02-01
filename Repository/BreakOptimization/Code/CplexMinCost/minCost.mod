/*********************************************
 * OPL 12.7.0.0 Model
 * Author: David Van Bulck
 * Based on Trick, M. A. Burke, E. & De Causmaecker, P. (Eds.) Integer and Constraint Programming 
 * Approaches for Round-Robin Tournament Scheduling Practice and Theory of Automated Timetabling IV, 
 * Springer, 2003, 63-77
 * Creation Date: Oct 18, 2017 at 10:08:47 AM
 *********************************************/
 // Parameters and variables 

int n=10; // Add other data file too project too
range Teams = 0..n-1;
range Slots = 1..n-1;
int cost[Teams][Teams][Slots]=...; 					// Cost is given
dvar int plays[Teams][Teams][Slots] in 0..1;	// 1 if team i plays home game against team j, 0 otherwise

 execute{
 	// Preprocessing 
    // Enable post processing    
    // Set time limit
    cplex.tilim = 43200;
 }

minimize sum(ordered i,j in Teams, s in Slots) cost[i][j][s]*plays[i][j][s];
subject to {
	//No team plays itself
	forall (i in Teams, t in Slots){
		plays[i][i][t] == 0;
	}		
	//Every team plays one game per slot
	//One-factor constraints
	forall (ordered i,j in Teams, t in Slots){
		plays[i,j,t] == plays[j,i,t];
	}	
	forall (i in Teams, t in Slots){
		sum (j in Teams) plays[i,j,t] == 1;
	}		
	//Every team plays every other team
	//All-different constraint
	forall (i,j in Teams: i!=j){
		sum(t in Slots) plays[i,j,t] == 1;
	}		
}

 execute{
 	// Postprocessing
	// Show solution each time one is found
	writeln("Save solution...");
	var ofile = new IloOplOutputFile("solMinCost"+n+".txt");
  	ofile.writeln(cplex.getObjValue());
	for(var k in Slots) {
		for(var i in Teams){
			for (var j = i+1; j < n; j++) { 		
				if(thisOplModel.plays[i][j][k] == 1){
					ofile.writeln("\t" + i + "\t" + j + "\t" + (k - 1));				
				}			
			}		
		}	
	}
  	ofile.close();
 }
