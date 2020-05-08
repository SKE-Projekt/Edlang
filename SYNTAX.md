# Syntax showcase
```
Int integer_variable;
String string_variable = '12';
Float float_variable = .23;

For(;;)
    Print(float_variable);
    Print('{1} ', float_variable);
Else
    # Użyteczne dla breakowania i trzymania flag
    Print('Pętla się zakończyła')
EndFor

Function sampleFunction (Int arg1, Float arg2) : Int
    Print(arg1);
    Print(arg2);
    # W celu wypisania wielu, trzeba skorzystać z formatu

    # Automatyczne kastowanie na lewy argument
    Return 12 * (arg1 / arg2);
EndFunction

If(true)
    Print("Prawda");
ElseIf(false)
    Print("Fałsz");
EndIf

Struct Person
    String name;
    Int age;

    Function Person : Int : {String name_v, Int age_v}
        # Konstruktor zwraca wartość w przypadku poprawnego utworzenia obiektu, albo fałsz w przeciwnym wypadku

        Self.name = name_v;
        Self.age_v = age_v;
        Return true;
    EndFunction
    # Destruktor na razie nie będzie potrzebny, nie ma zarządzania pamięcią specjalnego
EndStruct

```
