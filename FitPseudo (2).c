//PER IL ME FUTURO CHE NON RICORRDERÀ UN CAZZO DI ROOT

//QUESTO PROGRAMMA FITTA UN PICCO PSEUDOVOIGT, OVVERO UNA SOMMA PESATA TRA UNA GAUSSIANA E UNA LORETZIANA.
//PER FAR GIRARE IL PROGRAMMA DEVI CARICARE IL FILE CON:

//	.L FitPseudo.C

//E POI CHIAMARE SEMPRE SU TERMINALE LA FUNZIONE MAIN, CHE IN QUESTO CASO SI CHIAMA "pseudo()"

//BELLA 

Double_t lorentz(Double_t *x, Double_t *p) {
	
	Double_t pi; pi = 3.14159;
	
	return (1.0/pi)*(p[1])*(p[2]/((x[0]-p[0])*(x[0]-p[0])+p[2]*p[2]));
	
	//p0=mean  p1 = const  p2 = width
	
	}


Double_t mypol1(Double_t *x, Double_t *p) {
	
	return p[0] + x[0]*p[1];
	
	}


Double_t mygaus(Double_t *x, Double_t *p) {
	
	return p[1]*exp(-(0.5)*((x[0]-p[0])*(x[0]-p[0]))/(p[2]*p[2]));
	
	//p0=mean  p1 = const  p2 = width
	
	}
	
//QUI DEFINISCO LA PSEUDOVOIGT VERA E PROPRIA, DOVE POSSO CAMBIARE IL PESO f TRA LE DUE CURVE. 

//NB. Devi cambiare il valore di f anche nella main altrimenti non torna niente!!!

Double_t pv(Double_t *x, Double_t *p) {

	Double_t pi; pi = 3.14159;

	Double_t f;	f=0.9;

	return  f*(p[1]*exp(-(0.5)*((x[0]-p[0])*(x[0]-p[0]))/(p[2]*p[2])))   +   (1.0-f)*((1.0/pi)*(p[3])*(p[4]/((x[0]-p[0])*(x[0]-p[0])+p[4]*p[4]))) ;
	
}

	
Double_t pseudoV(Double_t *x, Double_t *p) {

	
	return mypol1(x,&p[0]) + pv(x,&p[2]);
	
	}


//PSEUDOVOIGDT FITTING ALGORITHM	
	
void pseudo() {

    TGraphErrors *graph=new TGraphErrors("GIXRD1_AuNP_om0p6_30-90_errors.xy","%lg %lg %lg","");

//DEF OF PERCENTAGE AND PARAMETER ARRAY

	Double_t f;	f=0.9;
	
	Double_t par[7];
		
//DEF OF LINEAR AND INITIAL VALUES

TF1 *fitLIN = new TF1("lin",mypol1, 30, 42,2);
  
    fitLIN->SetParameter(0,1300);
    fitLIN->SetParameter(1,-24);
    fitLIN->SetParNames("q1", "m1");	
        	
//DEF OF LORENTZIAN AND INITIAL VALUES

TF1 *fitLOR= new TF1("lorentz",lorentz, 34, 42,3);

    fitLOR->SetParameter(0,38);
    fitLOR->SetParameter(1,3000);
    fitLOR->SetParameter(2,0.5);
    fitLOR->SetParNames("meanL", "constL", "gamma");
    
//DEF OF GAUSSIAN AND INITIAL VALUES
       
TF1 *fitGAU = new TF1("Gaus",mygaus, 34, 42,3);
    
    fitGAU->SetParameter(0,38);
    fitGAU->SetParameter(1,3000);
    fitGAU->SetParameter(2,0.5);
    fitGAU->SetParNames("meanG", "constG","sigmaG");
 
//FIT OF LORENTZIAN, GAUSSIAN AND BG

       	graph->Fit(fitLIN,"R+","M");
       	graph->Fit(fitGAU,"R+","M");
       	graph->Fit(fitLOR,"R+","M");
       	
//SET OF LINE COLORS AND SAVING OF PARAMETERS OF SINGLE FUNCS 

Double_t aux[8];

    fitLIN->SetLineColor(5);	  
    fitLIN->GetParameters(&aux[0]); 	  
    	  
    fitGAU->SetLineColor(3);
    fitGAU->GetParameters(&aux[2]);
   
    fitLOR->SetLineColor(4);
    fitLOR->GetParameters(&aux[5]); 
    
//mannually filling par array


	par[0]=aux[0]; //q
	par[1]=aux[1]; //m
	par[2]=aux[5]; //MEAN  I use centroid of lorentzian because i like it more
	par[3]=aux[3]; //constG
	par[4]=aux[4]; //sigmaG
	par[5]=aux[6]; //constL
	par[6]=aux[7]; //gammaL  
    
         
//DEF OF FITTING FUNC, WHICH IS PSEUDOVOIGT

TF1 *fitFunc = new TF1("fitFunc",pseudoV,34,42,7);
   
   fitFunc->SetParNames("q", "m", "MEAN","constG", "SigmaG", "constL", "GammaL");
    fitFunc->SetLineColor(2); 
    
//SET PSEUDOVOIGT INITIAL VALUES AS FIT SERULTS OF SINGLE FUNCS

   fitFunc->SetParameters(par);
    
//SETTING BOUNDS FOR PARAMETERS---->NB ADJUST FOR EACH PEAK

    fitFunc->SetParLimits(2,36,40); //centroid
    fitFunc->SetParLimits(3,0,10E6); //constG
    fitFunc->SetParLimits(4,0,10E4); //sigmaG
    fitFunc->SetParLimits(5,0,10E6);  //constL
    fitFunc->SetParLimits(6,0,10E4); //gammaL
        
//FIT OF PSEUDOVOIGT AND SAVING OF RESULT PARAMETERS For PSEUDOVOGT

	graph->Fit(fitFunc,"R","M");
	fitFunc->GetParameters(par);
	
//INSERTING THE PARAMETERS FITTED BY (BG + PSEUDO) IN THE SINGLE FUNCS AND RENORMALIZING THEM TO PERCENTAGE


	fitLIN->SetParameters(par);
	
	fitGAU->SetParameter(0,par[2]);
	fitGAU->SetParameter(1,f*par[3]);
	fitGAU->SetParameter(2,par[4]);
	
	fitLOR->SetParameter(0, par[2]);
	fitLOR->SetParameter(1,(1.0-f)*par[5]);
	fitLOR->SetParameter(2,par[6]);
		
	
//INSERTING STAT BOX
    gStyle->SetOptFit(1);

//CREATE CANVAS
    
    TCanvas *d = new TCanvas("c2","XRD Experimental data",10,10,700,500);
    d->SetGrid();
   gStyle->SetGridColor(17);
   
    
//SET GRAPH SPECS     

    graph->SetTitle("XRD Experimental data");
    graph->GetXaxis()->SetTitle("Angle 2theta [grad]");
    graph->GetXaxis()->CenterTitle(true);
    graph->GetXaxis()->SetLabelSize(0.045);
    graph->GetXaxis()->SetTitleSize(0.045);
    graph->GetXaxis()->SetLimits(30,50);
    graph->GetYaxis()->SetTitle("Intensity [counts]");
    graph->GetYaxis()->CenterTitle(true);
    graph->GetYaxis()->SetLabelSize(0.045);
    graph->GetYaxis()->SetTitleSize(0.045);
    graph->GetYaxis()->SetTitleOffset(1.2);
    graph->SetMarkerStyle(2);
    //graph->FitPanel();
    graph->Draw();
  fitFunc->Draw("pl same");
  fitLIN->Draw("pl same");
  fitLOR->Draw("pl same");
  fitGAU->Draw("pl same");
    
//SET LEGEND SPECS
    	
   TLegend *legend=new TLegend(0.65,0.3,0.90,0.5);
    legend->SetHeader();
    legend->AddEntry(graph,"Experimental data","EP");
    legend->AddEntry(graph,"f = 0.9"); 
    legend->AddEntry(graph,"fit range is [34,42]");
    legend->AddEntry(fitFunc,"BG + PSEUDO","L");
    legend->AddEntry(fitLIN,"BG","L");
    legend->AddEntry(fitGAU,"f*Gaussian","L");
    legend->AddEntry(fitLOR,"(1-f)*Lorentzian","L");
    
    legend->Draw();

}
