REM This batch file sets up the necessary git remotes and aliases for managing subtrees in the InteractiveMap plugin repository.
@echo off
echo Setting up subtree remotes...
git remote add utils https://github.com/Miguel-Anastacio/UtilityPlugin.git
git remote add propDisp https://github.com/Miguel-Anastacio/PropertyDiplayGenerator.git     

echo.
echo Setting up aliases...
git config alias.push-subtrees "!git subtree push --prefix=Plugins/InteractiveMap/Source/PropDispGen propDisp main && git subtree push --prefix=Plugins/InteractiveMap/Source/Utils utils main"
git config alias.pull-subtrees "!git subtree pull --prefix=Plugins/InteractiveMap/Source/PropDispGen propDisp main --squash && git subtree pull --prefix=Plugins/InteractiveMap/Source/Utils utils main --squash"

echo.
echo Done! You can now use:
echo   git push-subtrees
echo   git pull-subtrees