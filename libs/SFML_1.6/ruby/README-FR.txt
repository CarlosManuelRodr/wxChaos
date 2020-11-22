Pour compiler/utiliser RubySFML sous Windows :

1) Installez Ruby pour Windows en utilisant l'installeur "one-click" (sur le site officiel de Ruby).
   (RubySFML ne peut pas l'utiliser car elle doit �tre recompil�e avec VS 2005,
    mais il doit tout de m�me �tre install� afin de pouvoir ex�cuter le Rakefile)

2) Ex�cutez la commande "gem install rake" pour installer la version Ruby de make.

3) Ouvrez une invite de commande DOS depuis ce r�pertoire

4) Assurez-vous que les variables d'environnement pour VS 2005 sont correctement d�finies. Si n�cessaire, ex�cutez le
   fichier suivant :
   <Chemin VS2005>\VC\bin\vcvars32.bat

5) Ex�cutez la commande "rake" depuis ce r�pertoire. La compilation par d�faut va :
   * T�l�charger / d�compresser / compiler / installer Ruby 1.8.6
   * Installer Ruby Gems (pas strictement n�cessaire, mais � peu pr�s tout le monde en aura besoin)
   * Configurer / compiler / installer RubySFML
   * Construire RubySFML.zip avec tout ce dont vous avez besoin pour distribuer vos binaires Windows.
     (Avec moins de 3.7 Mo, il contient tous les binaires et biblioth�ques Ruby plus tous les binaires SFML)

6) Maintenant, vous pouvez utiliser l'une de ces 3 commandes pour ex�cuter le programme de test :
   * test.bat
   * rake test
   * bin\ruby test.rb

NOTE : Cette compilation de Ruby ne contient pas toutes les extensions de l'installeur "one-click".
       Parmi les plus importantes manquantes, on peut noter DBI, OpenSSL et Expat.
