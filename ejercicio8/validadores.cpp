#include "validadores.h"

#include <QRegularExpression>

bool ValidadorSintaxis::balanceDelimitadores(const QString &linea) const {
    // Solo verifica que no haya CIERRE sin apertura correspondiente
    // Permite aperturas incompletas (normal en C++/Java)
    int parentesis = 0;
    int corchetes = 0;

    for (const QChar c : linea) {
        if (c == '(') {
            ++parentesis;
        } else if (c == ')') {
            --parentesis;
        } else if (c == '[') {
            ++corchetes;
        } else if (c == ']') {
            --corchetes;
        }

        // Si cierran sin apertura, es error
        if (parentesis < 0 || corchetes < 0) {
            return false;
        }
    }

    return true;  // Permite apertura incompleta
}

QString ValidadorCpp::nombreLenguaje() const {
    return "C++";
}

bool ValidadorCpp::validarLinea(const QString &linea, QString &diagnostico) const {
    const QString t = linea.trimmed();
    if (t.isEmpty() || t.startsWith("//")) {
        diagnostico = "Linea valida";
        return true;
    }

    // PALABRAS CLAVE PROHIBIDAS DE PYTHON
    static const QStringList palabrasPython = {
        "print", "def", "class", "import", "from", "if", "elif", "else", "for", "while",
        "try", "except", "with", "lambda", "return", "yield", "pass", "break", "continue",
        "in", "and", "or", "not", "is", "None", "True", "False", "self", "async", "await"
    };
    
    // PALABRAS CLAVE PROHIBIDAS DE JAVA
    static const QStringList palabrasJava = {
        "System", "public", "private", "protected", "static", "final", "abstract", "interface",
        "extends", "implements", "throw", "throws", "synchronized", "volatile", "transient",
        "new", "instanceof", "package", "import", "super", "this"
    };

    // Detectar palabras clave de Python
    for (const QString &palabra : palabrasPython) {
        if (t.contains(QRegularExpression("\\b" + palabra + "\\b"))) {
            diagnostico = "'" + palabra + "' es de Python, no de C++";
            return false;
        }
    }

    // Detectar palabras clave de Java
    for (const QString &palabra : palabrasJava) {
        if (palabra == "this" || palabra == "super") continue; // Pueden confundirse en C++
        if (t.contains(QRegularExpression("\\b" + palabra + "\\b"))) {
            diagnostico = "'" + palabra + "' es de Java, no de C++";
            return false;
        }
    }

    // Detectar patrones específicos
    if (t.contains(QRegularExpression(R"(:\s*$)"))) {  // Línea terminando con : (Python)
        diagnostico = "Las lineas con ':' al final son de Python. En C++ usa '{' '}'";
        return false;
    }

    if (!balanceDelimitadores(t)) {
        diagnostico = "Delimitadores desbalanceados en C++";
        return false;
    }

    static const QRegularExpression control(R"(^(if|for|while|switch|else|class|namespace|void|int|return)\b)");
    if (control.match(t).hasMatch()) {
        diagnostico = "Linea valida";
        return true;
    }

    if (!(t.endsWith(';') || t.endsWith('{') || t.endsWith('}') || t.endsWith(','))) {
        diagnostico = "En C++ la linea suele terminar con ';' o bloque '{ }'";
        return false;
    }

    diagnostico = "Linea valida";
    return true;
}

QString ValidadorPython::nombreLenguaje() const {
    return "Python";
}

bool ValidadorPython::validarLinea(const QString &linea, QString &diagnostico) const {
    const QString t = linea;
    const QString trimmed = t.trimmed();
    
    if (trimmed.isEmpty() || trimmed.startsWith('#')) {
        diagnostico = "Linea valida";
        return true;
    }

    // PALABRAS CLAVE PROHIBIDAS DE C++
    static const QStringList palabrasCpp = {
        "cout", "cin", "endl", "std", "void", "int", "char", "double", "float", "bool",
        "auto", "const", "static", "public", "private", "protected", "namespace", "include",
        "return", "using", "friend", "virtual", "override", "nullptr", "template", "typename",
        "struct", "enum", "union", "volatile", "extern", "inline", "register", "constexpr"
    };
    
    // PALABRAS CLAVE PROHIBIDAS DE JAVA
    static const QStringList palabrasJava = {
        "System", "public", "private", "protected", "static", "final", "abstract", "interface",
        "extends", "implements", "import", "package", "new", "instanceof", "synchronized",
        "volatile", "transient", "throw", "throws", "try", "catch", "finally"
    };

    // Detectar palabras clave de C++
    for (const QString &palabra : palabrasCpp) {
        if (trimmed.contains(QRegularExpression("\\b" + palabra + "\\b"))) {
            diagnostico = "'" + palabra + "' es de C++, no de Python";
            return false;
        }
    }

    // Detectar palabras clave de Java
    for (const QString &palabra : palabrasJava) {
        if (trimmed.contains(QRegularExpression("\\b" + palabra + "\\b"))) {
            diagnostico = "'" + palabra + "' es de Java, no de Python";
            return false;
        }
    }

    // Detectar patrones específicos de otros lenguajes
    if (trimmed.contains(QRegularExpression(R"(\{|}|;$)"))) {  // Bloques con {} o terminación con ;
        diagnostico = "Los bloques con '{}' y ';' son de C++/Java. En Python usa ':' y espacios";
        return false;
    }

    int espaciosIniciales = 0;
    while (espaciosIniciales < t.size() && t[espaciosIniciales] == ' ') {
        ++espaciosIniciales;
    }
    if (espaciosIniciales % 4 != 0) {
        diagnostico = "Indentacion sugerida en multiplos de 4 espacios";
        return false;
    }

    if (!balanceDelimitadores(trimmed)) {
        diagnostico = "Delimitadores desbalanceados en Python";
        return false;
    }

    static const QRegularExpression necesitaDosPuntos(R"(^(if|elif|else|for|while|def|class|try|except|with)\b)");
    if (necesitaDosPuntos.match(trimmed).hasMatch() && !trimmed.endsWith(':')) {
        diagnostico = "Bloques en Python deben finalizar con ':'";
        return false;
    }

    diagnostico = "Linea valida";
    return true;
}

QString ValidadorJava::nombreLenguaje() const {
    return "Java";
}

bool ValidadorJava::validarLinea(const QString &linea, QString &diagnostico) const {
    const QString t = linea.trimmed();
    if (t.isEmpty() || t.startsWith("//")) {
        diagnostico = "Linea valida";
        return true;
    }

    // PALABRAS CLAVE PROHIBIDAS DE C++
    static const QStringList palabrasCpp = {
        "cout", "cin", "endl", "std", "void", "int", "char", "double", "float", "bool",
        "auto", "const", "namespace", "include", "using", "virtual", "override", "nullptr",
        "template", "typename", "struct", "enum", "union", "volatile", "extern", "inline",
        "register", "constexpr", "friend", "nullptr", "size_t"
    };
    
    // PALABRAS CLAVE PROHIBIDAS DE PYTHON
    static const QStringList palabrasPython = {
        "print", "def", "class", "import", "from", "if", "elif", "else", "for", "while",
        "try", "except", "with", "lambda", "yield", "pass", "break", "continue",
        "in", "and", "or", "not", "is", "None", "True", "False", "self", "async", "await"
    };

    // Detectar palabras clave de C++
    for (const QString &palabra : palabrasCpp) {
        if (palabra == "int" || palabra == "const") continue; // Pueden aparecer en comentarios
        if (t.contains(QRegularExpression("\\b" + palabra + "\\b"))) {
            diagnostico = "'" + palabra + "' es de C++, no de Java";
            return false;
        }
    }

    // Detectar palabras clave de Python
    for (const QString &palabra : palabrasPython) {
        if (t.contains(QRegularExpression("\\b" + palabra + "\\b"))) {
            diagnostico = "'" + palabra + "' es de Python, no de Java";
            return false;
        }
    }

    // Detectar patrones específicos
    if (t.contains(QRegularExpression(R"(:\s*$)"))) {  // Línea terminando con : (Python)
        diagnostico = "Las lineas con ':' al final son de Python. En Java usa '{' '}'";
        return false;
    }

    // Detectar indentación con múltiplos de 4 (patrón de Python)
    if (t.startsWith("    ")) {
        int espacios = 0;
        for (const QChar c : t) {
            if (c == ' ') ++espacios;
            else break;
        }
        if (espacios % 4 == 0 && espacios > 0) {
            // Indentación en múltiplos de 4 es sospechoso en Java, pero permitir si parece legítimo
        }
    }

    if (!balanceDelimitadores(t)) {
        diagnostico = "Delimitadores desbalanceados en Java";
        return false;
    }

    static const QRegularExpression control(R"(^(if|for|while|switch|else|class|interface|public|private|protected|return)\b)");
    if (control.match(t).hasMatch()) {
        if (t.contains('{') || t.endsWith(';')) {
            diagnostico = "Linea valida";
            return true;
        }
    }

    if (!(t.endsWith(';') || t.endsWith('{') || t.endsWith('}'))) {
        diagnostico = "En Java la linea suele terminar con ';' o bloque '{ }'";
        return false;
    }

    diagnostico = "Linea valida";
    return true;
}
