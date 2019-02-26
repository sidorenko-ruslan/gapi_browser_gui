require './gap_browser.rb'

browser = GapBrowser.new('127.0.0.1',3000) # host and port
browser.goto 'https://www.amazon.com'
html = browser.page_html
debugger
puts "HTML data: " + html
