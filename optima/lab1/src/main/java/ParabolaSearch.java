import org.mariuszgromada.math.mxparser.Argument;
import org.mariuszgromada.math.mxparser.Expression;
import org.mariuszgromada.math.mxparser.Function;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class ParabolaSearch {

    private  static  double golrat = (1+Math.sqrt(5))/2;

    private static double eps = 0.000001;

    public static void setEps(double eps) {
        ParabolaSearch.eps = eps;
    }

    public static String findMin(double a, double b, String function) {

        DecimalFormatSymbols dfs = new DecimalFormatSymbols(Locale.getDefault());
        dfs.setDecimalSeparator('.');
        DecimalFormat d = new DecimalFormat("#.#######", dfs);


        Function f = new Function("F(x) = " + function);
        Argument x = new Argument("x = 0");
        Expression e = new Expression("F(x)", f, x);

        x.setArgumentValue(x1);
        double y1 = e.calculate();

        double x2 = (x1+x3)/2;

        x.setArgumentValue(x2);
        double y2 = e.calculate();

        x.setArgumentValue(x3);
        double y3 = e.calculate();

        if (x1 > x3)
            return "Некорректно заданы границы.";

        double delta = (x2-x1)*(x3-x1)*(x3-x2);

        double A, B, C, xmin;

        int iter = 0;

        do {
            A = 1/delta*((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3);
            B = 1/delta*(-(x3*x3-x2*x2)*y1+(x3*x3-x1*x1)*y2-(x2*x2-x1*x1)*y3);
            C = 1/delta*(x2*x3*(x3-x2)*y1-x1*x3*(x3-x1)*y2+x1*x2*(x2-x1)*y3);

            x.setArgumentValue(x1);
            fx1 = e.calculate();

            x.setArgumentValue(x2);
            fx2 = e.calculate();
            System.out.printf("fx1 = %f fx2 = %f\n", fx1, fx2);
            if (fx1 >= fx2)
                a = x1;
            else
                b = x2;
            iter++;
        }

        while (Math.abs(a - b) > eps);

        return "x = " + d.format((a+b)/2) + " i = " + iter;
    }

}
