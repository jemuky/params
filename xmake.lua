function download_archive()
    on_download(function (package, opt)
        import("net.http")
        import("utils.archive")


        local url = opt.url
        local sourcedir = opt.sourcedir
        local packagefile = path.filename(url)
        -- local sourcehash = package:sourcehash(opt.url_alias)

        -- print('url:',url,', sourcedir:', sourcedir, ', packagefile:', packagefile)

        -- attempt to remove package file first
        if not os.tryrm(packagefile) then
            print('try rm failed, file: ', packagefile)
        end
        http.download(url, packagefile)

        -- 提取压缩包
        if archive.extract(packagefile, sourcedir) then
            print('\n提取压缩包成功')
            os.tryrm(path.join(package:cachedir(), packagefile))
        else
            os.raise('extract fail, path: ', packagefile)
        end
    end)
end

package('ext_log')
    set_homepage('https://github.com/jemuky/log')
    set_license('MIT')

    -- github或gitee中"下载zip"复制链接地址
    add_urls('https://github.com/jemuky/log/archive/refs/tags/v0.0.1.tar.gz')
    add_versions('v0.0.1', '699c32dd66f5da4280a6119324da6ded858c44743c4548829e5ee2b2c133686e')

    set_cachedir(path.join(os.projectdir(), '3rd_download', 'log'))

    download_archive()

    on_install(function(package)
        os.cp('log.h', '$(projectdir)/build/incs/')
        os.cp('log.cpp', '$(projectdir)/build/src/')
        os.cd(package:cachedir())
        os.rmdir('source')
    end)
package_end()

add_requires('ext_log')

target('params_test')
    set_kind('binary')
    set_languages('cxx17')

    add_includedirs('build/incs')
    
    if is_plat('windows') then
        -- 编译选项
        add_cxflags('/utf-8')
    end

    add_files('build/src/*.cpp', '*.cpp')
target_end()
