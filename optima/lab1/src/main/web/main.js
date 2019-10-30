var calculator;

window.onload = function () {
    var elt = document.getElementById('calculator');
    calculator = Desmos.GraphingCalculator(elt, {
        keypad: false,
        settingsMenu: false,
        expressions: false
    });
    calculator.setExpression({id: 'graph1', latex: '1.62*x^3 - 8.15*x^2 + 4.39*x + 4.29'});
    calculator.setExpression({id: 'leftBorder'});
    calculator.setExpression({id: 'rightBorder'})
};

updateEquation = function (s) {
    calculator.setExpression({id: 'graph1', latex: s, color: Desmos.Colors.BLUE});
    calculator.setExpression({id: 'leftBorder', latex: ""});
    calculator.setExpression({id: 'rightBorder', latex: ""});
}

findRoot = function (lb, rb) {
    calculator.setExpression({id: 'leftBorder', latex: "x = " + lb, color: Desmos.Colors.RED});
    calculator.setExpression({id: 'rightBorder', latex: "x = " + rb, color: Desmos.Colors.RED});
};
