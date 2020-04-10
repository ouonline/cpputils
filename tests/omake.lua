project = CreateProject()

dep = project:CreateDependency()
dep:AddSourceFiles("*.cpp")
dep:AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")
dep:AddStaticLibrary("..", "text_utils_static")

target = project:CreateBinary("test_text_utils")
target:AddDependencies(dep)

return project
