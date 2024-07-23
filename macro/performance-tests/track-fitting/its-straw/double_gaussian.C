struct fitfunct {
    double n1;
    double sigma1;
    double mean;
    double n2;
    double sigma2;

    fitfunct() {
        n1 = 1.;
        n2 = 1.;
        mean = 0.;
        sigma1 = 0.01;
        sigma2 = 0.10;
    }

    // set parameters to function
    void init_f(TF1* f) {
        f->SetParameter(0, n1);
        f->SetParameter(1, mean);
        f->SetParameter(2, sigma1);
        f->SetParameter(3, n2);
        f->SetParameter(4, sigma2);
        f->SetParName(0, "c_{1}");
        f->SetParName(1, "#mu (GeV)");
        f->SetParName(2, "#sigma_{1} (GeV)");
        f->SetParName(3, "c_{2}");
        f->SetParName(4, "#sigma_{2} (GeV)");
    }

    // read parameters from function
    void init_from_f(TF1* f) {
        n1      = f->GetParameter(0);
        mean    = f->GetParameter(1);
        sigma1  = f->GetParameter(2);
        n2      = f->GetParameter(3);
        sigma2  = f->GetParameter(4);
    }

    double get_sigma() {
        double w1 = n1 / (n1+n2);
        double w2 = n2 / (n1+n2);

        return TMath::Sqrt(w1*sigma1*sigma1 + w2*sigma2*sigma2);
    }

    double get_sigma(TF1* f) {
        double n1 = f->GetParameter(0);
        double n2 = f->GetParameter(3);
        double s1 = f->GetParameter(2);
        double s2 = f->GetParameter(4);

        double w1 = n1 / (n1+n2);
        double w2 = n2 / (n1+n2);

        return w1*s1 + w2*s2;
    }

    void print() {
        cout << n1 << endl;
        cout << mean << endl;
        cout << sigma1 << endl;
        cout << n2 << endl;
        cout << sigma2 << endl;
    }

    double operator()(double* x, double* par) {
        n1      = par[0];
        mean    = par[1];
        sigma1  = par[2];
        n2      = par[3];
        sigma2  = par[4];
        double M = x[0];
        return n1 * TMath::Gaus(M, mean, sigma1, true) + n2 * TMath::Gaus(M, mean, sigma2, true);
    }
};


struct FitResults {
    double mean;
    double sigma;
};

// Fitter of TH1 histogram, also plots the results
FitResults fit_double_gaussian( TH1* h, int color = kBlue,
                           TString  first_draw_opt = "E",
                           double xmin = -0.15, double xmax = 0.15 ) {

    h->SetMarkerStyle(20);
    h->Draw(first_draw_opt);
    h->GetXaxis()->SetRangeUser(xmin, xmax);

    fitfunct ff;
    TF1 fitf("fitf", ff, xmin, xmax, 5);
    ff.init_f(&fitf);
    fitf.SetLineColor(color);
    auto resptr = h->Fit("fitf", "SL");
    for (int i=0; i<5; i++) {
        cout << fitf.GetParameter(i) << endl;
    }

    ff.init_from_f(&fitf);
    fitfunct g1 = ff;
    fitfunct g2 = ff;
    g1.n2 = 0;
    g2.n1 = 0;
    TF1* fitf_g1 = new TF1("fitf_g1", g1, xmin, xmax, 5);
    g1.init_f(fitf_g1);
    fitf_g1->SetLineStyle(2);
    fitf_g1->SetLineColor(color);
    fitf_g1->Draw("same");

    TF1* fitf_g2 = new TF1("fitf_g2", g2, xmin, xmax, 5);
    g2.init_f(fitf_g2);
    fitf_g2->SetLineStyle(2);
    fitf_g2->SetLineColor(color);
    fitf_g2->Draw("same");

    TLatex l;
    l.SetNDC();
    l.DrawLatex( 0.6, 0.7, Form("#sigma_{1} ~ %2.2f%%", ff.sigma1*1e2 ));
    l.DrawLatex( 0.6, 0.6, Form("#sigma_{2} ~ %2.2f%%", ff.sigma2*1e2) );
    l.DrawLatex( 0.6, 0.5, Form("#sigma_{w} ~ %2.2f%%", ff.get_sigma()*1e2) );


    cout << "Weighted sigma: " << ff.get_sigma( &fitf ) << endl;

    FitResults res;
    res.mean  = ff.mean;
    res.sigma = ff.get_sigma();

    return res;
}
