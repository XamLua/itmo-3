import org.mariuszgromada.math.mxparser.Argument;
import org.mariuszgromada.math.mxparser.Expression;
import org.mariuszgromada.math.mxparser.Function;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

import static java.lang.Double.isNaN;

public class ParabolaSearch {

    private  static  double golrat = (1+Math.sqrt(5))/2;

    private static double eps = 0.000001;

    public static void setEps(double eps) {
        ParabolaSearch.eps = eps;
    }

    public static String findMin(double x1, double x3, String function) {

        DecimalFormatSymbols dfs = new DecimalFormatSymbols(Locale.getDefault());
        dfs.setDecimalSeparator('.');
        DecimalFormat d = new DecimalFormat("#.#######", dfs);


        Function f = new Function("F(x) = " + function);
        Argument x = new Argument("x = 0");
        Expression e = new Expression("F(x)", f, x);

        double x2 = x3 - (x3-x1)/golrat;

        if (x1 > x3)
            return "Некорректно заданы границы.";

        double A, B, C, xmin = x2, ymin, y1, y2, y3, delta, prevxmin;

        int iter = 0;

        do {
            prevxmin = xmin;

            x.setArgumentValue(x1);
            y1 = e.calculate();

            while(isNaN(y1)){
                x1+=eps;
                x.setArgumentValue(x1);
                y1 = e.calculate();
            }

            x.setArgumentValue(x2);
            y2 = e.calculate();

            x.setArgumentValue(x3);
            y3 = e.calculate();

            delta = (x2-x1)*(x3-x1)*(x3-x2);
            A = ((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3)/delta;
            B = (-(x3*x3-x2*x2)*y1+(x3*x3-x1*x1)*y2-(x2*x2-x1*x1)*y3)/delta;
            C = (x2*x3*(x3-x2)*y1-x1*x3*(x3-x1)*y2+x1*x2*(x2-x1)*y3)/delta;

            xmin = -B/(2*A);
            System.out.printf("x1 = %f, x2 = %f, x3 = %f, xmin = %f\n", x1, x2, x3, xmin);
            x.setArgumentValue(xmin);
            ymin = e.calculate();

            if (xmin >= x2 && ymin < y2){
                x1 = x2;
                x2 = xmin;
            }
            else if (xmin >= x2 && ymin >= y2){
                x3 = xmin;
            }
            else if (xmin < x2 && ymin < y2){
                x3 = x2;
                x2 = xmin;
            }
            else if (xmin < x2 && ymin >= y2){
                x1 = xmin;
            }

            iter++;
        }
        while (Math.abs(xmin - prevxmin) > eps);
        return "x = " + d.format(xmin) + " i = " + iter;
    }

}