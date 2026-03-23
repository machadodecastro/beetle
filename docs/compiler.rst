Compiler
==========

The Beetle compiler transforms Beetle code into C.

Pipeline
--------

1. Lexer
2. Parser
3. AST
4. C code generation
5. Compilation with GCC

Exemple
-------

Beetle:

.. code-block:: beetle

   print("Hi")

C generated:

.. code-block:: c

   printf("Hi");
