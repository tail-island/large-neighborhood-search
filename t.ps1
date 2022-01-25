foreach($item in Get-ChildItem .\data\*.txt)
{
    Write-Host $item

    Get-Content $item | .\build\Release\large-neighborhood-search.exe | python extra/visualize_solution.py $item
}