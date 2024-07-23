struct fitfunct {
    double a1;
    double sigma1;
    double mean;
    double a2;
    double sigma2;

    fitfunct() {
        a1 = 1.;
        a2 = 1.;
        mean = 3.097;
        sigma1 = 0.020;
        sigma2 = 0.050;
    }

    void init_f(TF1* f) {
        f->SetParameter(0, a1);
        f->SetParameter(1, mean);
        f->SetParameter(2, sigma1);
        f->SetParameter(3, a2);
        f->SetParameter(4, sigma2);
        f->SetParName(0, "C_{1}");
        f->SetParName(1, "#mu (GeV)");
        f->SetParName(2, "#sigma_{1} (GeV)");
        f->SetParName(3, "C_{2}");
        f->SetParName(4, "#sigma_{2} (GeV)");
    }

    void init_from_f(TF1* f) {
        a1      = f->GetParameter(0);
        mean    = f->GetParameter(1);
        sigma1  = f->GetParameter(2);
        a2      = f->GetParameter(3);
        sigma2  = f->GetParameter(4);
    }

    double get_sigma(TF1* f) {
        double a1 = f->GetParameter(0);
        double a2 = f->GetParameter(3);
        double s1 = f->GetParameter(2);
        double s2 = f->GetParameter(4);

        double w1 = a1 / (a1+a2);
        double w2 = a2 / (a1+a2);

        return w1*s1 + w2*s2;
    }

    void print() {
        cout << a1 << endl;
        cout << mean << endl;
        cout << sigma1 << endl;
        cout << a2 << endl;
        cout << sigma2 << endl;
    }

    double operator()(double* x, double* par) {
        a1      = par[0];
        mean    = par[1];
        sigma1  = par[2];
        a2      = par[3];
        sigma2  = par[4];
        double M = x[0];
        return a1 * TMath::Gaus(M, mean, sigma1, true) + a2 * TMath::Gaus(M, mean, sigma2, true);
    }
};

void FitDimuSpectrum( TH1D* hMdimu ) {

    hMdimu->SetMarkerStyle(20);
    hMdimu->SetTitle("Dimuon mass spectrum fitted with the double Gaussian shape");
    hMdimu->Draw("E");

    fitfunct ff;
    TF1 fitf("fitf", ff, 2.8, 3.3, 5);
    ff.init_f(&fitf);
    auto resptr = hMdimu->Fit("fitf", "S");
    for (int i=0; i<5; i++) {
        cout << fitf.GetParameter(i) << endl;
    }

    ff.init_from_f(&fitf);
    fitfunct g1 = ff;
    fitfunct g2 = ff;
    g1.a2 = 0;
    g2.a1 = 0;
    TF1* fitf_g1 = new TF1("fitf_g1", g1, 2.8, 3.3, 5);
    g1.init_f(fitf_g1);
    fitf_g1->SetLineStyle(2);
    fitf_g1->Draw("same");

    TF1* fitf_g2 = new TF1("fitf_g2", g2, 2.8, 3.3, 5);
    g2.init_f(fitf_g2);
    fitf_g2->SetLineStyle(2);
    fitf_g2->Draw("same");

    TLatex l;
    l.SetNDC();
    l.DrawLatex( 0.6, 0.7, Form("#sigma_{1} ~ %2.f MeV", fitf.GetParameter(2)*1e3) );
    l.DrawLatex( 0.6, 0.6, Form("#sigma_{2} ~ %2.f MeV", fitf.GetParameter(4)*1e3) );


    cout << ff.get_sigma( &fitf ) << endl;
}
