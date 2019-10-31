import org.mariuszgromada.math.mxparser.Argument;
import org.mariuszgromada.math.mxparser.Expression;
import org.mariuszgromada.math.mxparser.Function;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class GSSearch {

    private  static  double golrat = (1+Math.sqrt(5))/2;

    private static double eps = 0.1;

    public static void setEps(double eps) {
        GSSearch.eps = eps;
    }

    public static String findMin(double a, double b, String function) {

        DecimalFormatSymbols dfs = new DecimalFormatSymbols(Locale.getDefault());
        dfs.setDecimalSeparator('.');
        DecimalFormat d = new DecimalFormat("#.#######", dfs);


        Function f = new Function("F(x) = " + function);
        Argument x = new Argument("x = 0");
        Expression e = new Expression("F(x)", f, x);

        x.setArgumentValue(a);
        double fa = e.calculate();

        x.setArgumentValue(b);
        double fb = e.calculate();

        if (a > b)
            return "Некорректно заданы границы.";

        double x1, x2, fx1, fx2;
        int iter = 0;

        do {
            x1 = b - (b-a)/golrat;
            x2 = a + (b-a)/golrat;
            System.out.printf("a = %f b = %f x1 = %f x2 = %f\n", a, b, x1, x2);
            x.setArgumentValue(x1);
            fx1 = e.calculate();

            x.setArgumentValue(x2);
            fx2 = e.calculate();
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